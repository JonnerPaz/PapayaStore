import type { HandlerEvent, HandlerContext } from "@netlify/functions";

function onStar(payload: any): string {
  const { action, sender, repository } = payload;
  return `User ${sender.login} ${action} star on ${repository.full_name}`;
}

function onIssue(payload: any): string {
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

function onPush(payload: any) {
  const { before, after, repository, sender, forced } = payload;
  const message = `User ${sender.login} pushed to ${repository.full_name} from ${before.slice(0, 7)} to ${after.slice(0, 7)}`;
  if (forced) {
    return `BEWARE: ${message} (FORCED PUSH)`;
  }

  return message;
}

const notify = async (message: string, img?: string) => {
  const body = { content: message };

  const resp = await fetch(process.env.DISCORD_WEBHOOK_URL ?? "", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify(body),
  });

  if (!resp.ok) {
    console.log("Error sending message to discord");
    console.log("Response", resp);
    return false;
  }

  return true;
};

async function handler(event: HandlerEvent, context: HandlerContext) {
  const githubEvent = event.headers["x-github-event"] ?? "unknown";
  const payload = JSON.parse(event.body ?? "{}");
  let message: string;

  console.log({ githubEvent, payload });

  switch (githubEvent) {
    case "star":
      message = onStar(payload);
      break;

    case "issues":
      message = onIssue(payload);
      break;

    case "push":
      message = onPush(payload);

    default:
      message = `Unknown event ${githubEvent}`;
  }

  await notify(message);

  return {
    statusCode: 200,
    body: JSON.stringify({ message }),
    headers: {
      "Content-Type": "application/json",
    },
  };
}

export { handler };
