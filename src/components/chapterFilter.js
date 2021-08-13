import React, { Component } from "react";
import { connect } from "react-redux";
import { filterChapter, readChapter } from "../redux/actions/chapterActions";
import store from "../redux/store/store";

class ChapterFilter extends Component {
  constructor(props) {
    super(props);
    this.state = {
      prevChapter: null,
      filter: "",
      filtered: false,
    };
  }

  handleChange = (event) => {
    this.setState({ filter: event.target.value });
  };

  handleSubmit = (e) => {
    e.preventDefault();
    if (this.state.filter != "") {
      this.props.handleFilterChapter(this.state.filter);
      if (!this.state.filtered) this.setState({ filtered: true });
    } else {
      if (this.state.filtered) {
        this.props.handleUpdateChapter();
        this.setState({ filtered: false });
      }
    }
  };

  componentDidMount() {
    store.subscribe(() => {
      const prev = this.state.prevChapter;
      if (
        !this.props.loading &&
        this.props.chapter.name != undefined &&
        this.props.chapter.name != this.state.prevChapter
      ) {
        this.setState({
          prevChapter: this.props.chapter.name,
          filter: "",
          filtered: false,
        });
      }
    });
  }

  render() {
    return (
      <>
        {this.props.visible && (
          <form className="row g-3">
            <div className="col-auto">
              <input
                type="text"
                className="form-control"
                id="filter"
                name="filter"
                value={this.state.filter}
                onChange={this.handleChange}
              ></input>
            </div>
            <div className="col-auto">
              <button
                onClick={this.handleSubmit}
                className="btn btn-success mb-3"
              >
                Search
              </button>
            </div>
          </form>
        )}
      </>
    );
  }
}

//subscribing to redux store updates
const mapStateToProps = ({ chapter, loading }) => ({
  chapter,
  loading,
});

export default connect(mapStateToProps, {
  filterChapter,
  readChapter,
})(ChapterFilter);
