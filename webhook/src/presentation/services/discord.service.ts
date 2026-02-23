process.loadEnvFile();

interface BodyResponse {
  content: string;
}

export class DiscordService {
  private readonly discordWebhookUrl = process.env.DISCORD_WEBHOOK_URL ?? "";

  constructor() {}

  async notify(message: string, img?: string) {
    const body: BodyResponse = { content: message };

    const resp = await fetch(this.discordWebhookUrl, {
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
  }
}
