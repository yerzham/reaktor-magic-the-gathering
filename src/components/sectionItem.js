import React, { Component } from "react";
import { NavLink } from "react-router-dom";

//component that renders individual menu item stored in redux store
export default class SectionItem extends Component {
  constructor(props) {
    super(props);
    this.state = {
      name: "",
      contents: null,
      children: [],
    };
  }

  //setting necessary values on mounting of component
  componentDidMount() {
    const { name, contents, children } = this.props;
    this.setState({ name, contents, children });
  }

  render() {
    return (
      <>
        <li className="mb-1">
          <button
            className="btn btn-toggle align-items-center rounded collapsed"
            type="button"
            data-bs-toggle="collapse"
            data-bs-target={"#collapse" + this.state.name}
            aria-expanded="false"
            aria-controls="collapseExample"
          >
            {this.state.contents}
          </button>
          <div className="collapse" id={"collapse" + this.state.name}>
            <ul className="btn-toggle-nav list-unstyled fw-normal pb-1 small">
              {this.state.children.map((chapter) => {
                return (
                  <li key={chapter.name}>
                    <NavLink
                      className="rounded"
                      key={chapter.name + "link"}
                      to={"/chapter/" + chapter.name}
                    >
                      {chapter.contents}
                    </NavLink>
                  </li>
                );
              })}
            </ul>
          </div>
        </li>
      </>
    );
  }
}
