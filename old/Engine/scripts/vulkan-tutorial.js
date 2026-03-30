//
// PURPOSE OF THIS SCRIPT IS TO ADD DARK MODE TO https://vulkan-tutorial.com
//

// BOOKMARKLET CREATION STEPS
//
// Create the Bookmark
// ----------
// - Open your browser's bookmarks manager.
// - Add a new bookmark or edit an existing one.
// - Set the bookmark's URL field to the javascript: code (below)
//
// Run the Bookmarklet
// ----------
// - Open vulkan-tutorial.com
// - Click the bookmark you just created.
// - Your JavaScript should execute on the page.

javascript: (function () {
  document.getElementsByClassName("content")[0].style.background = "#333333";
  document.getElementsByClassName("content-page")[0].style.background =
    "#333333";

  const pTags = document
    .getElementsByClassName("doc_content")[0]
    .getElementsByTagName("p");

  for (let i = 0; i < pTags.length; ++i) {
    pTags[i].style.color = "white";
  }

  const h1Tags = document
    .getElementsByClassName("doc_content")[0]
    .getElementsByTagName("h1");

  for (let i = 0; i < h1Tags.length; ++i) {
    h1Tags[i].style.color = "white";
  }

  const h2Tags = document
    .getElementsByClassName("doc_content")[0]
    .getElementsByTagName("h2");

  for (let i = 0; i < h2Tags.length; ++i) {
    h2Tags[i].style.color = "white";
  }

  const h3Tags = document
    .getElementsByClassName("doc_content")[0]
    .getElementsByTagName("h3");

  for (let i = 0; i < h3Tags.length; ++i) {
    h3Tags[i].style.color = "white";
  }

  const h4Tags = document
    .getElementsByClassName("doc_content")[0]
    .getElementsByTagName("h4");

  for (let i = 0; i < h4Tags.length; ++i) {
    h4Tags[i].style.color = "white";
  }

  const liTags = document
    .getElementsByClassName("doc_content")[0]
    .getElementsByTagName("li");

  for (let i = 0; i < liTags.length; ++i) {
    liTags[i].style.color = "white";
  }
})();
