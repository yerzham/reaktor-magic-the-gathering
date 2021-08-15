import React, { Component } from "react";
import SectionItem from "./sectionItem";
import { connect } from "react-redux";
import { readTableOfContents } from "../redux/actions/sectionActions";
import axios from "axios";

//main component that wraps major part of application
class TableOfContents extends Component {
  constructor(props) {
    super(props);
    this.state = {};
  }

  componentDidMount() {
    this.props.readTableOfContents();
  }

  render() {
    return (
      <>
        <div
          className={"sidebar p-3" + (this.props.show ? " shown" : " hidden")}
        >
          <h1 className="d-flex align-items-center pb-3 mb-3 nav-title text-decoration-none border-bottom">
            <span className="fs-5 fw-semibold">Table of Contents</span>
          </h1>
          <div className="list-unstyled ps-0">
            {this.props.sections.length > 0 ? (
              this.props.sections.map((section, i) => {
                return (
                  <SectionItem
                    key={section.name}
                    name={section.name}
                    contents={section.contents}
                    children={section.children}
                  />
                );
              })
            ) : (
              <div className="menu-row">
                <div className="msg">List is empty.</div>
              </div>
            )}
          </div>
        </div>
      </>
    );
  }
}

//subscribing to redux store updates
const mapStateToProps = ({ sections }) => ({
  sections,
});

//connecting our main component to redux store
export default connect(mapStateToProps, {
  readTableOfContents,
})(TableOfContents);
