// firebase.js

// Import necessary Firebase modules
import { initializeApp } from "https://www.gstatic.com/firebasejs/9.6.5/firebase-app.js";
import { 
  getAuth, 
  createUserWithEmailAndPassword, 
  signInWithEmailAndPassword, 
  updateProfile, 
  GoogleAuthProvider, 
  signInWithPopup,
  onAuthStateChanged
} from "https://www.gstatic.com/firebasejs/9.6.5/firebase-auth.js";

// Firebase configuration object
const firebaseConfig = {
  apiKey: "AIzaSyBr_e-l-NwAg7B5CEyw7BKLL8q6dUBonMI",
  authDomain: "silix-7aeb2.firebaseapp.com",
  projectId: "silix-7aeb2",
  storageBucket: "silix-7aeb2.appspot.com",
  messagingSenderId: "70606744538",
  appId: "1:70606744538:web:ba87689a646f363890eee3"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const auth = getAuth(app);
const googleProvider = new GoogleAuthProvider();

// Function to check if user is logged in
export function checkLoginStatus() {
  return new Promise((resolve, reject) => {
    onAuthStateChanged(auth, (user) => {
      if (user) {
        resolve(true); // User is logged in
      } else {
        resolve(false); // User is not logged in
      }
    }, reject);
  });
}

// Function to sign in with email and password
export async function signInWithEmailAndPasswordHandler(email, password) {
  try {
    const userCredential = await signInWithEmailAndPassword(auth, email, password);
    return userCredential.user;
  } catch (error) {
    throw error;
  }
}

// Function to sign up with email and password
export async function signUpWithEmailAndPasswordHandler(name, email, password) {
  try {
    const userCredential = await createUserWithEmailAndPassword(auth, email, password);
    await updateProfile(userCredential.user, { displayName: name });
    return userCredential.user;
  } catch (error) {
    throw error;
  }
}

// Function to sign in with Google
export async function signInWithGoogleHandler() {
  try {
    const result = await signInWithPopup(auth, googleProvider);
    return result.user;
  } catch (error) {
    throw error;
  }
}

export default app;
