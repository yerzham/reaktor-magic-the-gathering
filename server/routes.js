const { fetchContents, fetchChapter, fetchRule } = require("./controller");

const routes = [
  {
    method: "GET",
    url: "/api/",
    schema: {
      response: {
        200: {
          type: "array",
        },
      },
    },
    handler: fetchContents,
  },
  {
    method: "GET",
    url: "/api/chapter/:chapter",
    schema: {
      response: {
        200: {
          type: "object",
          properties: {
            name: { type: "string" },
            contents: { type: "string" },
            children: { type: "array" },
          },
        },
      },
    },
    handler: fetchChapter,
  },
  {
    method: "GET",
    url: "/api/rule/:rule",
    schema: {
      response: {
        200: {
          type: "string",
        },
      },
    },
    handler: fetchRule,
  },
];

module.exports = routes;
