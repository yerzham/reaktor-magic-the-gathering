const fastify = require("fastify")({ logger: true });
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
    await fastify.listen(3000);
  } catch (err) {
    fastify.log.error(err);
    process.exit(1);
  }
};
start();
