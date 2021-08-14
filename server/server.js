#!/usr/bin/env node

const fastify = require("fastify")({
  ignoreTrailingSlash: true,
  logger: true,
});
const routes = require("./routes");
const path = require("path");
const DistPath = path.join(__dirname, "..", "dist");

routes.forEach((route) => fastify.route(route));

fastify.register(require("fastify-static"), {
  root: DistPath,
});

fastify.get("/chapter/:chapter", async (request, reply) => {
  await reply.sendFile("index.html");
});

fastify.get("/rule/:rule", async (request, reply) => {
  await reply.sendFile("index.html");
});

const start = async () => {
  try {
    if (process.env.NODE_ENV == "production") {
      await fastify.listen(3000, "0.0.0.0", (err, address) => {
        if (err) {
          fastify.log.error(err);
          process.exit(1);
        }
      });
    }
    await fastify.listen(3000);
  } catch (err) {
    fastify.log.error(err);
    process.exit(1);
  }
};
start();
