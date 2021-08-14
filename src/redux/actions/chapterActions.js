import axios from "axios";
import { fetchSectionsSuccess } from "./sectionActions";

export const FETCH_CHAPTER_BEGIN = "FETCH_CHAPTER_BEGIN";
export const FETCH_CHAPTER_SUCCESS = "FETCH_CHAPTER_SUCCESS";
export const FETCH_CHAPTER_FAILURE = "FETCH_CHAPTER_FAILURE";
export const FETCH_CHAPTER_CANCELLED = "FETCH_CHAPTER_CANCELLED";

export const FILTER_CHAPTER_SUCCESS = "FILTER_CHAPTER_SUCCESS";

const CancelToken = axios.CancelToken;
var source = CancelToken.source();

//disptached when we fetch items from database
export const fetchChapterBegin = () => ({
  type: FETCH_CHAPTER_BEGIN,
});

//dispatched when items are successfully fetched
export const fetchChapterFailure = (error) => ({
  type: FETCH_CHAPTER_FAILURE,
  payload: { error },
});

//dispatched when chapter is successfully fetched
export const fetchChapterSuccess = (chapter) => ({
  type: FETCH_CHAPTER_SUCCESS,
  payload: { chapter },
});

export const fetchChapterCancelled = () => ({
  type: FETCH_CHAPTER_CANCELLED,
});

export const cancelChapterRequest = () => {
  return (dispatch) => {
    source.cancel("Operation cancelled by the user.");
    source = CancelToken.source();
    dispatch(fetchChapterCancelled());
  };
};

export const readChapter = (chapter) => {
  return (dispatch) => {
    dispatch(fetchChapterBegin());
    return axios
      .get("/api/chapter/" + chapter, {
        cancelToken: source.token,
      })
      .then(({ data }) => {
        dispatch(fetchChapterSuccess(data));
      })
      .catch(
        (error) =>
          !axios.isCancel(error) && dispatch(fetchChapterFailure(error))
      );
  };
};

export const filterChapter = (chapter, filter) => {
  return (dispatch) => {
    dispatch(fetchChapterBegin());
    return axios
      .get("/api/chapter/" + chapter, {
        params: {
          filter,
        },
        cancelToken: source.token,
      })
      .then(({ data }) => {
        dispatch(fetchChapterSuccess(data));
      })
      .catch((error) => dispatch(fetchChapterFailure(error)));
  };
};
