import React, { Component } from "react";
import { HashLink } from "react-router-hash-link";
import axios from "axios";

class RuleIsolated extends Component {
  constructor(props) {
    super(props);
    this.state = {
      rule: "",
      notFound: false,
      error: null,
    };
  }

  getRuleChapter() {
    const re = /([0-9]+)\./g;
    const chapter = (this.props.match.params.rule.match(re) || []).join("");
    return chapter.substring(0, chapter.length - 1);
  }

  componentDidMount() {
    const rule = this.props.match.params.rule || "";
    axios
      .get("/api/rule/" + rule)
      .then(({ data }) => {
        this.setState({ rule: data });
      })
      .catch((error) =>
        error.response.status
          ? this.setState({ notFound: true, error })
          : this.setState({ error })
      );
  }

  render() {
    return (
      <>
        {this.state.error == null ? (
          <>
            <h1>{this.props.match.params.rule}</h1>
            <p>{this.state.rule}</p>
            <HashLink
              to={
                "/chapter/" +
                this.getRuleChapter() +
                "#" +
                this.props.match.params.rule
              }
            >
              See in context
            </HashLink>
          </>
        ) : this.state.notFound == true ? (
          <h1>The rule was not found</h1>
        ) : (
          <h1>
            Unexpected Error. Website is probably broken. Send Help... please
          </h1>
        )}
      </>
    );
  }
}

export default RuleIsolated;
