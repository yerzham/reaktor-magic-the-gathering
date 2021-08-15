import React, { Component } from "react";

class Navbar extends Component {
  constructor(props) {
    super(props);
    this.state = { toggle: false };
  }

  handleToggle = (e) => {
    this.setState({ toggle: !this.state.toggle });
    this.props.handleSidebarToggle(!this.state.toggle);
  };

  render() {
    return (
      <nav className="navbar navbar-light bg-light">
        <div className="container-fluid ">
          <div className="d-flex">
            <button
              className={
                "navbar-toggler" + (this.state.toggle ? " active" : " disabled")
              }
              onClick={this.handleToggle}
              type="button"
            >
              <span className="navbar-toggler-icon"></span>
            </button>
            <a className="navbar-brand" href="#">
              Magic: The Gathering
            </a>
          </div>
        </div>
      </nav>
    );
  }
}

export default Navbar;
