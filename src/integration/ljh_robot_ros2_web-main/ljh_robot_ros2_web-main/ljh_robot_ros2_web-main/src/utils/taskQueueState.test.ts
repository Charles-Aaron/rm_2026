import { describe, expect, it } from 'vitest';
import { NAV_STATUS } from './navStatus';
import { applyNavStatusToTaskQueue } from './taskQueueState';

describe('taskQueueState', () => {
  it('starts first waiting task when nav is active', () => {
    const tasks = [
      { status: '等待' as const },
      { status: '等待' as const },
    ];

    const updated = applyNavStatusToTaskQueue(tasks, NAV_STATUS.EXECUTING);
    expect(updated[0]!.status).toBe('进行中');
    expect(updated[1]!.status).toBe('等待');
  });

  it('advances queue on success', () => {
    const tasks = [
      { status: '进行中' as const },
      { status: '等待' as const },
      { status: '等待' as const },
    ];

    const updated = applyNavStatusToTaskQueue(tasks, NAV_STATUS.SUCCEEDED);
    expect(updated[0]!.status).toBe('已完成');
    expect(updated[1]!.status).toBe('进行中');
  });

  it('returns task to waiting on aborted', () => {
    const tasks = [
      { status: '已完成' as const },
      { status: '进行中' as const },
    ];

    const updated = applyNavStatusToTaskQueue(tasks, NAV_STATUS.ABORTED);
    expect(updated[1]!.status).toBe('等待');
  });
});
