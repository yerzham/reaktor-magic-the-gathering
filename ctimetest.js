const RuleTextService = require("./build/Release/RuleTextService");

console.time("C++ File Processing");
const cdata = RuleTextService.open("text.txt");
console.timeEnd("C++ File Processing");

console.time("C++ Filter");
var filtered1 = RuleTextService.rules_containing("deck");
console.timeEnd("C++ Filter");

console.time("C++ Search Chapter By ID");
var filtered2 = RuleTextService.chapter("100");
console.timeEnd("C++ Search Chapter By ID");

console.time("C++ Filter Chapter By ID");
var filtered3 = RuleTextService.chapter_rules_containing("100", "tournament");
console.timeEnd("C++ Filter Chapter By ID");

console.time("C++ Search Rule By ID");
var filtered4 = RuleTextService.rule("102.1");
console.timeEnd("C++ Search Rule By ID");

const used = process.memoryUsage();
for (let key in used) {
  console.log(`${key} ${Math.round((used[key] / 1024 / 1024) * 100) / 100} MB`);
}
