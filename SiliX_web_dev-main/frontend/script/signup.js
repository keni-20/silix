// signup.js

import { signUpWithEmailAndPasswordHandler, signInWithGoogleHandler } from './firebase.js';

document.addEventListener('DOMContentLoaded', () => {
    const signUpForm = document.getElementById('signup-form');
    const googleSignUpButton = document.getElementById('google-signup');

    signUpForm.addEventListener('submit', async function(event) {
        event.preventDefault();

        const name = document.getElementById('name').value;
        const email = document.getElementById('email').value;
        const password = document.getElementById('password').value;
        const confirmPassword = document.getElementById('confirm-password').value;

        if (!name || !email || !password || !confirmPassword) {
            alert('Please fill in all fields.');
            return;
        }

        if (password !== confirmPassword) {
            alert('Passwords do not match.');
            return;
        }

        try {
            // Create user with email and password
            const user = await signUpWithEmailAndPasswordHandler(name, email, password);

            // Optionally, redirect or show a success message
            alert('Sign up successful!'); // Show a success message
            window.location.href = 'index.html'; // Redirect to success page

            console.log('User signed up:', user);
        } catch (error) {
            console.error('Error signing up:', error.message);
            alert(error.message);
        }
    });

    googleSignUpButton.addEventListener('click', async () => {
        try {
            const user = await signInWithGoogleHandler();
            console.log('User signed in with Google:', user);
            
            // Redirect or show a success message
            window.location.href = 'index.html'; // Redirect to success page
            alert('Google Sign-In successful!'); // Show a success message
        } catch (error) {
            console.error('Error during Google Sign-In:', error.message);
            alert(error.message);
        }
    });
});
