import React, { Component, useEffect } from "react";
import { withRouter } from "react-router";
import {
  readChapter,
  filterChapter,
  cancelChapterRequest,
} from "../redux/actions/chapterActions";
import { connect } from "react-redux";
import RuleItem from "./ruleItem";
import ChapterFilter from "./chapterFilter";
import store from "../redux/store/store";

//component that renders individual menu item stored in redux store
class ChapterItem extends Component {
  constructor(props) {
    super(props);
    this.state = { filtering: false, chapterInView: null };
  }

  static updateChapter(props, state) {
    props.cancelChapterRequest();
    if (props.chapter.name != undefined && !window.location.hash)
      document.getElementById("canvas").scrollTop = 0;
    state.chapterInView = props.match.params.chapter || "";
    props.readChapter(state.chapterInView);
    return state;
  }

  handleUpdateChapter = () => {
    this.props.readChapter(this.props.match.params.chapter);
  };

  handleFilterChapter = (filter) => {
    this.setState({ filtering: true });
    this.props.filterChapter(this.props.match.params.chapter, filter);
  };

  //setting necessary values on mounting of component
  componentDidMount() {
    const listenWhileLoadToHash = store.subscribe(() => {
      if (!this.props.loadingChapter && window.location.hash) {
        const id = window.location.hash.replace("#", "");
        const element = document.getElementById(id);
        if (element != null) {
          listenWhileLoadToHash();
          element.scrollIntoView();
        }
      } else if (!window.location.hash) {
        listenWhileLoadToHash();
      }
    });

    store.subscribe(() => {
      if (!this.props.loadingChapter && this.state.filtering) {
        this.setState({ filtering: false });
      }
    });
  }

  static getDerivedStateFromProps(props, state) {
    if (
      !props.loadingChapter &&
      props.match.params.chapter != undefined &&
      props.match.params.chapter != state.chapterInView
    ) {
      state = ChapterItem.updateChapter(props, state);
    }
    return state;
  }

  render() {
    return (
      <>
        {!this.props.chapterError ? (
          <>
            {!this.props.chapter.contents != undefined &&
              !(this.props.loadingChapter && !this.state.filtering) && (
                <h1>{this.props.chapter.contents}</h1>
              )}

            <ChapterFilter
              visible={
                this.props.chapter.children != undefined &&
                !this.props.loadingChapter
              }
              handleUpdateChapter={this.handleUpdateChapter}
              handleFilterChapter={this.handleFilterChapter}
            ></ChapterFilter>

            <div className="demo">
              {!this.props.loadingChapter &&
                this.props.chapter.children != undefined &&
                (this.props.chapter.children.length != 0 ? (
                  <>
                    {this.props.chapter.children.map((rule) => {
                      return <RuleItem key={rule.name} rule={rule}></RuleItem>;
                    })}
                  </>
                ) : (
                  this.props.chapter.children == undefined ||
                  (this.props.chapter.children.length == 0 && <div></div>)
                ))}
            </div>
          </>
        ) : (
          <h1>Chapter does not exist</h1>
        )}
      </>
    );
  }
}

//subscribing to redux store updates
const mapStateToProps = ({ chapter, chapterError, loadingChapter }) => ({
  chapter,
  chapterError,
  loadingChapter,
});

export default withRouter(
  connect(mapStateToProps, {
    readChapter,
    filterChapter,
    cancelChapterRequest,
  })(ChapterItem)
);
