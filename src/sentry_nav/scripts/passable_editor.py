#!/usr/bin/env python3
"""Offline editor for map-frame passable polygons."""
import argparse
import math
from pathlib import Path
import tkinter as tk
from tkinter import messagebox

import yaml
from PIL import Image, ImageTk


class Editor:
    def __init__(self, root, map_file, output):
        self.root, self.output = root, Path(output)
        config = yaml.safe_load(Path(map_file).read_text())
        self.res = float(config['resolution'])
        self.ox, self.oy, self.yaw = map(float, config['origin'])
        image_file = Path(config['image'])
        if not image_file.is_absolute():
            image_file = Path(map_file).resolve().parent / image_file
        self.original = Image.open(image_file).convert('RGB')
        self.width, self.height = self.original.size
        self.fit_scale = min(1.0, 1200 / self.width, 750 / self.height)
        self.scale = self.fit_scale
        # Cap bitmap size to avoid allocating enormous images for large maps.
        self.max_scale = max(self.fit_scale, min(16.0, 12000 / self.width,
                                                  12000 / self.height,
                                                  math.sqrt(64000000 / (self.width * self.height))))
        self.min_scale = min(self.fit_scale, 0.05)
        self.image = None
        self.regions, self.pending = [], []
        if self.output.exists():
            data = yaml.safe_load(self.output.read_text())
            if data['version'] != 1 or data['frame'] != 'map':
                raise ValueError('标注文件格式或坐标系不正确')
            self.regions = [r['vertices'] for r in data['regions']]
        root.title('离线通行区标注 — 区域内忽略所有障碍')
        bar = tk.Frame(root); bar.pack(fill='x')
        for label, fn in [('完成多边形', self.finish), ('撤销顶点', self.undo), ('删除最后区域', self.delete), ('保存', self.save), ('放大 +', lambda: self.zoom_center(1.25)), ('缩小 −', lambda: self.zoom_center(0.8)), ('适应窗口', self.reset_view)]:
            tk.Button(bar, text=label, command=fn).pack(side='left')
        tk.Label(root, text='左键画点；滚轮缩放；右键拖动或使用滚动条平移。红色区域内所有障碍均被忽略。').pack(anchor='w')
        view = tk.Frame(root); view.pack(fill='both', expand=True)
        self.canvas = tk.Canvas(view, width=min(1200, max(600, round(self.width*self.scale))),
                                height=min(750, max(400, round(self.height*self.scale))),
                                cursor='crosshair', highlightthickness=0)
        xscroll = tk.Scrollbar(view, orient='horizontal', command=self.canvas.xview)
        yscroll = tk.Scrollbar(view, orient='vertical', command=self.canvas.yview)
        self.canvas.configure(xscrollcommand=xscroll.set, yscrollcommand=yscroll.set)
        self.canvas.grid(row=0, column=0, sticky='nsew')
        yscroll.grid(row=0, column=1, sticky='ns'); xscroll.grid(row=1, column=0, sticky='ew')
        view.grid_rowconfigure(0, weight=1); view.grid_columnconfigure(0, weight=1)
        self.canvas.bind('<Button-1>', self.click)
        self.canvas.bind('<ButtonPress-3>', lambda event: self.canvas.scan_mark(event.x, event.y))
        self.canvas.bind('<B3-Motion>', lambda event: self.canvas.scan_dragto(event.x, event.y, gain=1))
        self.canvas.bind('<MouseWheel>', self.mousewheel)
        self.canvas.bind('<Button-4>', lambda event: self.zoom_at(1.25, event.x, event.y))
        self.canvas.bind('<Button-5>', lambda event: self.zoom_at(0.8, event.x, event.y))
        self.render_image()
        self.draw()

    def pixel_to_world(self, px, py):
        x = px / self.scale * self.res
        y = (self.height - py / self.scale) * self.res
        c, s = math.cos(self.yaw), math.sin(self.yaw)
        return [round(self.ox + c*x - s*y, 6), round(self.oy + s*x + c*y, 6)]

    def world_to_pixel(self, p):
        dx, dy = p[0] - self.ox, p[1] - self.oy
        c, s = math.cos(self.yaw), math.sin(self.yaw)
        return ((c*dx+s*dy)/self.res*self.scale, (self.height-(-s*dx+c*dy)/self.res)*self.scale)

    def render_image(self):
        size = (max(1, round(self.width * self.scale)), max(1, round(self.height * self.scale)))
        self.image = ImageTk.PhotoImage(self.original.resize(size, Image.NEAREST))
        self.canvas.configure(scrollregion=(0, 0, *size))

    def zoom_at(self, factor, x, y):
        new_scale = max(self.min_scale, min(self.max_scale, self.scale * factor))
        if new_scale == self.scale:
            return
        # Keep the map coordinate under the pointer fixed as the image grows.
        image_x = self.canvas.canvasx(x) / self.scale
        image_y = self.canvas.canvasy(y) / self.scale
        self.scale = new_scale
        self.render_image()
        self.draw()
        self.canvas.update_idletasks()
        image_width, image_height = self.image.width(), self.image.height()
        self.canvas.xview_moveto((image_x * new_scale - x) / image_width)
        self.canvas.yview_moveto((image_y * new_scale - y) / image_height)

    def zoom_center(self, factor):
        self.zoom_at(factor, self.canvas.winfo_width() / 2, self.canvas.winfo_height() / 2)

    def mousewheel(self, event):
        if event.delta:
            self.zoom_at(1.25 if event.delta > 0 else 0.8, event.x, event.y)
        return 'break'

    def reset_view(self):
        self.scale = self.fit_scale
        self.render_image()
        self.draw()
        self.canvas.xview_moveto(0); self.canvas.yview_moveto(0)

    def draw(self):
        self.canvas.delete('all'); self.canvas.create_image(0, 0, anchor='nw', image=self.image)
        for region in self.regions:
            pts = [v for p in region for v in self.world_to_pixel(p)]
            self.canvas.create_polygon(*pts, fill='', outline='red', width=3)
        pts = [v for p in self.pending for v in self.world_to_pixel(p)]
        if len(pts) >= 4: self.canvas.create_line(*pts, fill='orange', width=3)
        for x, y in zip(pts[::2], pts[1::2]): self.canvas.create_oval(x-4, y-4, x+4, y+4, fill='orange')

    def click(self, event):
        px, py = self.canvas.canvasx(event.x), self.canvas.canvasy(event.y)
        if 0 <= px < self.width * self.scale and 0 <= py < self.height * self.scale:
            self.pending.append(self.pixel_to_world(px, py)); self.draw()

    def finish(self):
        if len(self.pending) < 3:
            messagebox.showerror('无法完成', '多边形至少需要三个顶点'); return
        self.regions.append(self.pending); self.pending = []; self.draw()

    def undo(self):
        if self.pending: self.pending.pop(); self.draw()

    def delete(self):
        if self.regions and messagebox.askyesno('删除区域', '删除最后一个已完成区域？'):
            self.regions.pop(); self.draw()

    def save(self):
        if self.pending:
            messagebox.showerror('无法保存', '请先完成或撤销正在绘制的多边形'); return
        data = {'version': 1, 'frame': 'map', 'regions': [{'vertices': r} for r in self.regions]}
        self.output.write_text(yaml.safe_dump(data, allow_unicode=True, sort_keys=False))
        messagebox.showinfo('已保存', str(self.output.resolve()))


def main():
    parser = argparse.ArgumentParser(description='离线标注忽略障碍的区域')
    parser.add_argument('map', help='Nav2 地图 YAML')
    parser.add_argument('output', help='通行区域 YAML 输出路径')
    args = parser.parse_args()
    if not args.output.strip():
        parser.error('标注文件路径不能为空或只包含空格')
    root = tk.Tk()
    try: Editor(root, args.map, args.output)
    except Exception as exc:
        messagebox.showerror('加载失败', str(exc)); root.destroy(); raise
    root.mainloop()


if __name__ == '__main__': main()
