import axios from "axios";

//action types created and exported
export const READ_TABLE_OF_CONTENTS = "fetch table of contents";

export const FETCH_SECTIONS_BEGIN = "begin fetching items";
export const FETCH_SECTIONS_SUCCESS = "Items fetched successfully";
export const FETCH_SECTIONS_FAILURE = "Failed to fetch items";

//disptached when we fetch items from database
export const fetchSectionsBegin = () => ({
  type: FETCH_SECTIONS_BEGIN,
});

//dispatched when items are successfully fetched
export const fetchSectionsSuccess = (items) => ({
  type: FETCH_SECTIONS_SUCCESS,
  payload: { items },
});

//dispatched when items are fails to load
export const fetchSectionsFailure = (error) => ({
  type: FETCH_SECTIONS_FAILURE,
  payload: { error },
});

//dispatched when all the items stored in redux store needs to be read
export const readTableOfContents = () => {
  return (dispatch) => {
    dispatch(fetchSectionsBegin());
    return axios
      .get("/api/")
      .then(({ data }) => {
        console.log(data);
        console.log("success");
        dispatch(fetchSectionsSuccess(data));
      })
      .catch((error) => dispatch(fetchSectionsFailure(error)));
  };
};
