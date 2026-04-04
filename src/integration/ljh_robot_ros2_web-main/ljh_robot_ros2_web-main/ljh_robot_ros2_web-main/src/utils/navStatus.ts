export const NAV_STATUS = {
  UNKNOWN: 0,
  ACCEPTED: 1,
  EXECUTING: 2,
  CANCELING: 3,
  SUCCEEDED: 4,
  CANCELED: 5,
  ABORTED: 6,
} as const;

export type NavStatusCode = (typeof NAV_STATUS)[keyof typeof NAV_STATUS];

export function isNavSucceeded(status: number | undefined): boolean {
  return status === NAV_STATUS.SUCCEEDED;
}

export function isNavAborted(status: number | undefined): boolean {
  return status === NAV_STATUS.ABORTED;
}

export function isNavCanceled(status: number | undefined): boolean {
  return status === NAV_STATUS.CANCELED;
}

export function isNavActive(status: number | undefined): boolean {
  return status === NAV_STATUS.ACCEPTED || status === NAV_STATUS.EXECUTING || status === NAV_STATUS.CANCELING;
}

export function toDeviceNavLabel(status: number | undefined): '规划中' | '跟踪中' | '到达' | '待机' {
  if (status === NAV_STATUS.ACCEPTED) {
    return '规划中';
  }
  if (status === NAV_STATUS.EXECUTING || status === NAV_STATUS.CANCELING) {
    return '跟踪中';
  }
  if (status === NAV_STATUS.SUCCEEDED) {
    return '到达';
  }
  return '待机';
}
