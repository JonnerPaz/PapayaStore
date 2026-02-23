import express from "express";
import { GithubController } from "./presentation/github/controller.js";

process.loadEnvFile();

async function main() {
  const app = express();
  const controller = new GithubController();

  app.use(express.json());
  app.post("/api/github", controller.webhookHandler);

  app.listen(process.env.PORT, () => {
    console.log(`App running on port ${process.env.PORT}`);
  });
}

await main();
