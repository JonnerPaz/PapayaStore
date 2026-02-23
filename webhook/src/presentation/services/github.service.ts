import type { GitHubStarPayload } from "../../interfaces/github-start.interface.js";
import type { GitHubIssuePayload } from "../../interfaces/github-issue.interface.js";
import type { GithubPushPayload } from "../../interfaces/github-push.interface.js";

export class GitHubService {
  constructor() {}

  onStar(payload: GitHubStarPayload): string {
    const { action, sender, repository } = payload;
    return `User ${sender.login} ${action} star on ${repository.full_name}`;
  }

  onIssue(payload: GitHubIssuePayload): string {
    const { action, issue } = payload;

    if (action === "opened") {
      return `An issue was opened with this title ${issue.title}`;
    }

    if (action === "closed") {
      return `An issue was closed by ${issue.user.login}`;
    }

    if (action === "reopened") {
      return `An issue was reopened by ${issue.user.login}`;
    }

    return `Unhandled action for the issue event ${action}`;
  }

  onPush(payload: GithubPushPayload) {
    const { before, after, repository, sender, forced } = payload;
    const message = `User ${sender.login} pushed to ${repository.full_name} from ${before.slice(0, 7)} to ${after.slice(0, 7)}`;
    if (forced) {
      return `BEWARE: ${message} (FORCED PUSH)`;
    }

    return message;
  }
}
