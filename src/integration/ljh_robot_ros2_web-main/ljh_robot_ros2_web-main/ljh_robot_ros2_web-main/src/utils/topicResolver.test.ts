import { describe, expect, it } from 'vitest';
import { ROBOT_NAMESPACE, stripRobotNamespace, withRobotNamespace } from './topicResolver';

describe('topicResolver', () => {
  it('adds namespace for robot topics', () => {
    expect(withRobotNamespace('/map')).toBe(`${ROBOT_NAMESPACE}/map`);
    expect(withRobotNamespace('cmd_vel')).toBe(`${ROBOT_NAMESPACE}/cmd_vel`);
  });

  it('keeps global topics untouched', () => {
    expect(withRobotNamespace('/rosout')).toBe('/rosout');
  });

  it('keeps already namespaced topics untouched', () => {
    expect(withRobotNamespace(`${ROBOT_NAMESPACE}/odometry`)).toBe(`${ROBOT_NAMESPACE}/odometry`);
  });

  it('strips namespace for display', () => {
    expect(stripRobotNamespace(`${ROBOT_NAMESPACE}/web/nav_status`)).toBe('/web/nav_status');
  });
});
