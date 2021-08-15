import React from "react";
import { Link } from "react-router-dom";
import { HashLink } from "react-router-hash-link";

const RuleItem = ({ rule, isSubrule }) => {
  const re = /([0-9]+\.[0-9]+[a-z]{0,1})|([0-9]{3,})/g;

  var matches = [];
  var match;
  var pos = 0;
  while ((match = re.exec(rule.contents.slice(7))) != null) {
    matches.push({ match: match[0], index: match.index });
  }
  function updatePos(index, length) {
    pos = index + 7 + length;
    return "";
  }

  return (
    <React.Fragment key={rule.name + "fragment1"}>
      <p
        key={rule.name + "paragtaph"}
        id={rule.name}
        style={isSubrule == true ? { paddingLeft: "15px" } : {}}
      >
        {matches.map((match) => {
          return (
            <React.Fragment key={match.index + "fragment2"}>
              {rule.contents.slice(pos, match.index + 7)}
              {updatePos(match.index, match.match.length)}
              {match.match.includes(".") ? (
                <HashLink
                  key={rule.name + "hashlink"}
                  to={"/rule/" + match.match}
                >
                  {match.match}
                </HashLink>
              ) : (
                <Link key={rule.name + "link"} to={"/chapter/" + match.match}>
                  {match.match}
                </Link>
              )}
            </React.Fragment>
          );
        })}
        {rule.contents.slice(pos)}
      </p>
      {rule.children.map((subrule) => {
        return (
          <RuleItem
            isSubrule={true}
            key={subrule.name}
            rule={subrule}
          ></RuleItem>
        );
      })}
    </React.Fragment>
  );
};

export default RuleItem;
