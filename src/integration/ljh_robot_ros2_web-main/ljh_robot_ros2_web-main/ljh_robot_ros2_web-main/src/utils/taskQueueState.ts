import { NAV_STATUS } from './navStatus';

export type QueueTaskStatus = '等待' | '进行中' | '已完成';

export interface QueueTaskLike {
  status: QueueTaskStatus;
}

export function applyNavStatusToTaskQueue<T extends QueueTaskLike>(tasks: T[], status: number | undefined): T[] {
  if (tasks.length === 0 || status === undefined) {
    return tasks;
  }

  const next = tasks.map(task => ({ ...task }));
  const inProgressIndex = next.findIndex(task => task.status === '进行中');

  if (status === NAV_STATUS.ACCEPTED || status === NAV_STATUS.EXECUTING || status === NAV_STATUS.CANCELING) {
    if (inProgressIndex === -1) {
      const nextWaitingIndex = next.findIndex(task => task.status === '等待');
      if (nextWaitingIndex >= 0) {
        next[nextWaitingIndex]!.status = '进行中';
      }
    }
    return next;
  }

  if (status === NAV_STATUS.SUCCEEDED) {
    if (inProgressIndex >= 0) {
      next[inProgressIndex]!.status = '已完成';
      const nextWaitingIndex = next.findIndex(task => task.status === '等待');
      if (nextWaitingIndex >= 0) {
        next[nextWaitingIndex]!.status = '进行中';
      }
    } else {
      const nextWaitingIndex = next.findIndex(task => task.status === '等待');
      if (nextWaitingIndex >= 0) {
        next[nextWaitingIndex]!.status = '已完成';
      }
    }
    return next;
  }

  if (status === NAV_STATUS.ABORTED || status === NAV_STATUS.CANCELED) {
    if (inProgressIndex >= 0) {
      next[inProgressIndex]!.status = '等待';
    }
    return next;
  }

  return next;
}
