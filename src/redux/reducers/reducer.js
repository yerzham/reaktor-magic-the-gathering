import { READ_TABLE_OF_CONTENTS } from "../actions/sectionActions";
import {
  FETCH_SECTIONS_BEGIN,
  FETCH_SECTIONS_SUCCESS,
  FETCH_SECTIONS_FAILURE,
} from "../actions/sectionActions";
import {
  FETCH_CHAPTER_BEGIN,
  FETCH_CHAPTER_SUCCESS,
  FETCH_CHAPTER_FAILURE,
  FETCH_CHAPTER_CANCELLED,
} from "../actions/chapterActions";

//initial state for redux store
const initialState = {
  sections: [],
  sectionsError: null,
  chapter: {},
  chapterError: null,
};

//reducer function
export default function (state = initialState, action) {
  switch (action.type) {
    case FETCH_SECTIONS_BEGIN:
      return {
        ...state,
        loading: true,
        sectionsError: null,
      };

    case FETCH_SECTIONS_SUCCESS:
      return {
        ...state,
        loading: false,
        sections: action.payload.items,
      };

    case FETCH_SECTIONS_FAILURE:
      return {
        ...state,
        loading: false,
        sectionsError: action.payload.error,
      };

    case FETCH_CHAPTER_BEGIN:
      return {
        ...state,
        loadingChapter: true,
        chapterError: null,
      };

    case FETCH_CHAPTER_CANCELLED:
      return {
        ...state,
        loadingChapter: false,
        chapterError: null,
      };

    case FETCH_CHAPTER_SUCCESS:
      return {
        ...state,
        loadingChapter: false,
        chapter: action.payload.chapter,
      };

    case FETCH_CHAPTER_FAILURE:
      return {
        ...state,
        loadingChapter: false,
        chapterError: action.payload.error,
      };

    //returns default state, in case some unknown action type is discovered
    default:
      return state;
  }
}
