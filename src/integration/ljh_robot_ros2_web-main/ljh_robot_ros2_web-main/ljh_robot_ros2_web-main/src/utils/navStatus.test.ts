import { describe, expect, it } from 'vitest';
import { NAV_STATUS, isNavAborted, isNavSucceeded, toDeviceNavLabel } from './navStatus';

describe('navStatus', () => {
  it('uses ROS2 action status code mapping', () => {
    expect(isNavSucceeded(NAV_STATUS.SUCCEEDED)).toBe(true);
    expect(isNavAborted(NAV_STATUS.ABORTED)).toBe(true);
  });

  it('maps status to device labels', () => {
    expect(toDeviceNavLabel(NAV_STATUS.ACCEPTED)).toBe('规划中');
    expect(toDeviceNavLabel(NAV_STATUS.EXECUTING)).toBe('跟踪中');
    expect(toDeviceNavLabel(NAV_STATUS.SUCCEEDED)).toBe('到达');
    expect(toDeviceNavLabel(undefined)).toBe('待机');
  });
});
