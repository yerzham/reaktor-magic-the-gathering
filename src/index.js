import React, { Component } from "react";
import "./menu.css";
import ReactDOM from "react-dom";
import { BrowserRouter as Router, Switch, Route } from "react-router-dom";
import TableOfContents from "./components/tableOfContents";
import { Provider } from "react-redux";
import store from "./redux/store/store";
import ChapterItem from "./components/chapterItem";
import RuleIsolated from "./components/ruleIsolated";
import LoadingOverlay from "./components/loadingOverlay";
import Navbar from "./components/navbar";

//element where we'll mount our react app
const rootElement = document.getElementById("root");

class App extends Component {
  constructor(props) {
    super(props);
    this.state = {
      showSideBar: false,
    };
  }

  toggleSidebar = (toggle) => {
    this.setState({ showSideBar: toggle });
  };

  render() {
    return (
      // making our redux store available to nested components.
      <Provider store={store}>
        <Router>
          <LoadingOverlay></LoadingOverlay>
          <Navbar handleSidebarToggle={this.toggleSidebar}></Navbar>
          <div className="page">
            <TableOfContents show={this.state.showSideBar} />
            <div className="canvas" id="canvas">
              <div className="p-5 canvas-inner">
                <Switch>
                  <Route
                    exact
                    path="/chapter/:chapter"
                    component={ChapterItem}
                  ></Route>
                  <Route
                    exact
                    path="/rule/:rule"
                    component={RuleIsolated}
                  ></Route>
                  <Route path="/">
                    <h1>Welcome. Feel free to explore the game rules.</h1>
                  </Route>
                </Switch>
              </div>
            </div>
          </div>
        </Router>
      </Provider>
    );
  }
}

//we render our app over rootElement
ReactDOM.render(<App />, rootElement);
