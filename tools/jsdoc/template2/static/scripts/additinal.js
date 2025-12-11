(function() {
    
// Theme toggle button
document.getElementById('theme-toggle').addEventListener('click', function() {
    //document.body.classList.toggle('dark-theme');

    console.log('Theme toggle button clicked');

    if (document.body.className === "dark-mode") {
        document.body.className = "";
    } else {
        document.body.className = "dark-mode";
    }
});

}());

