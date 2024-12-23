// login.js

import { signInWithEmailAndPasswordHandler, signInWithGoogleHandler } from './firebase.js';

document.addEventListener('DOMContentLoaded', () => {
    const form = document.querySelector('form');

    // Handle form submission for email/password login
    form.addEventListener('submit', async function(event) {
        event.preventDefault(); // Prevent form from submitting normally

        const email = form.querySelector('input[type="email"]').value;
        const password = form.querySelector('input[type="password"]').value;

        if (!email || !password) {
            alert('Please fill in all fields.');
            return;
        }

        try {
            const user = await signInWithEmailAndPasswordHandler(email, password);
            
            // Redirect or show a success message
            window.location.href = 'index.html'; // Redirect to dashboard page
            alert('Login successful!'); // Show a success message

            console.log('User logged in:', user);
        } catch (error) {
            console.error('Error signing in:', error.message);
            alert(error.message);
        }
    });

    // Handle Google sign-in
    document.getElementById('google-login').addEventListener('click', async () => {
        try {
            const user = await signInWithGoogleHandler();
            console.log('User signed in with Google:', user);

            // Redirect or show a success message
            window.location.href = 'index.html'; // Redirect to dashboard page
            alert('Google Sign-In successful!'); // Show a success message
        } catch (error) {
            console.error('Error signing in with Google:', error.message);
            alert(error.message);
        }
    });
});
