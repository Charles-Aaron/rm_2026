export const ROBOT_NAMESPACE = '/red_standard_robot1';

const GLOBAL_TOPICS = new Set<string>(['/rosout', '/clock']);

export function normalizeTopic(topic: string): string {
  return topic.startsWith('/') ? topic : `/${topic}`;
}

export function isGlobalTopic(topic: string): boolean {
  const normalized = normalizeTopic(topic);
  return GLOBAL_TOPICS.has(normalized);
}

export function withRobotNamespace(topic: string | null | undefined): string | null {
  if (!topic) {
    return null;
  }

  const normalized = normalizeTopic(topic);
  if (isGlobalTopic(normalized)) {
    return normalized;
  }

  if (normalized === ROBOT_NAMESPACE || normalized.startsWith(`${ROBOT_NAMESPACE}/`)) {
    return normalized;
  }

  return `${ROBOT_NAMESPACE}${normalized}`;
}

export function stripRobotNamespace(topic: string): string {
  const normalized = normalizeTopic(topic);
  if (normalized.startsWith(`${ROBOT_NAMESPACE}/`)) {
    return normalized.slice(ROBOT_NAMESPACE.length);
  }
  return normalized;
}

export function withOptionalRobotNamespace(topic: string, useNamespace: boolean = true): string {
  const normalized = normalizeTopic(topic);
  if (!useNamespace) {
    return normalized;
  }
  return withRobotNamespace(normalized) ?? normalized;
}
