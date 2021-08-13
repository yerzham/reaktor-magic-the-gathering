import React, { Component } from "react";
import { connect } from "react-redux";

class LoadingOverlay extends Component {
  render() {
    return (
      <>
        {this.props.loading ? (
          <div className="loading-overlay" style={{ display: "inherit" }}>
            <div
              className="loading-overlay-image-container"
              style={{ display: "inherit" }}
            >
              <div className="spinner-border" role="status">
                <span className="visually-hidden">Loading...</span>
              </div>
            </div>
          </div>
        ) : (
          <div className="loading-overlay">
            <div className="loading-overlay-image-container">
              <div className="spinner-border" role="status">
                <span className="visually-hidden">Loading...</span>
              </div>
            </div>
          </div>
        )}
      </>
    );
  }
}

const mapStateToProps = ({ loading }) => ({
  loading,
});

export default connect(mapStateToProps, {})(LoadingOverlay);
