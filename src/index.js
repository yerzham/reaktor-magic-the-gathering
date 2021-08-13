import React from "react";
import "./menu.css";
import ReactDOM from "react-dom";
import { BrowserRouter as Router, Switch, Route } from "react-router-dom";
import TableOfContents from "./components/tableOfContents";
import { Provider } from "react-redux";
import store from "./redux/store/store";
import ChapterItem from "./components/chapterItem";
import RuleIsolated from "./components/ruleIsolated";
import LoadingOverlay from "./components/loadingOverlay";

//element where we'll mount our react app
const rootElement = document.getElementById("root");

const App = () => {
  return (
    // making our redux store available to nested components.
    <Provider store={store}>
      <Router>
        <LoadingOverlay></LoadingOverlay>
        <TableOfContents />
        <div className="canvas flex-fill" id="canvas">
          <div className="flex-fill p-5 canvas-inner">
            <Switch>
              <Route
                exact
                path="/chapter/:chapter"
                component={ChapterItem}
              ></Route>
              <Route exact path="/rule/:rule" component={RuleIsolated}></Route>
              <Route path="/">
                <h1>Welcome</h1>
                <h1>Welcome</h1>
                <h1>Welcome</h1>
                <h1>Welcome</h1>
                <h1>Welcome</h1>
              </Route>
            </Switch>
          </div>
        </div>
      </Router>
    </Provider>
  );
};

//we render our app over rootElement
ReactDOM.render(<App />, rootElement);
