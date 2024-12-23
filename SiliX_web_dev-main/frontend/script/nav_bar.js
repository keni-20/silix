// Import necessary Firebase methods from firebase.js
import { checkLoginStatus } from './firebase.js';

window.onload = async function() {
    const isLoggedIn = await checkLoginStatus();
    const protectedLinks = document.querySelectorAll('.protected-link');
    const profileLink = document.getElementById('profileLink');

    if (!isLoggedIn) {
        const redirectToLogin = () => {
            // Create an <a> element to perform a _top redirect
            const loginLink = document.createElement('a');
            loginLink.href = 'login.html';
            loginLink.target = '_top';
            loginLink.style.display = 'none'; // Hide the link
            document.body.appendChild(loginLink); // Append it to the body
            loginLink.click(); // Programmatically click the link
            document.body.removeChild(loginLink); // Remove the link after click
        };

        protectedLinks.forEach(link => {
            link.addEventListener('click', function(event) {
                event.preventDefault(); // Prevent default link action
                alert('Please log in or sign up to access this page.'); // Notify user
                redirectToLogin(); // Redirect to login page
            });
        });

        profileLink.addEventListener('click', function(event) {
            event.preventDefault(); // Prevent default link action
            alert('Please log in to access your profile.'); // Notify user
            redirectToLogin(); // Redirect to login page
        });
    }
};
