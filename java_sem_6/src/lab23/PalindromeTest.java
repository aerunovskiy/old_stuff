package lab23;

import org.junit.jupiter.api.Test;

import static lab23.Palindrome.isPalindrome;
import static org.junit.jupiter.api.Assertions.*;

class PalindromeTest {
    @Test
    public void testIsPalindrome() {
        assertTrue(isPalindrome("madam"));
        assertTrue(isPalindrome("Madam"));
        assertTrue(isPalindrome("12321"));
        assertTrue(isPalindrome(" "));
        assertTrue(isPalindrome(" a"));
        assertTrue(isPalindrome("b "));
        assertTrue(isPalindrome("!@#@!"));

        assertTrue (isPalindrome("Marge let a moody baby doom a telegram"));
        assertFalse(isPalindrome("I am just a regular everyday normal guy"));

        assertFalse(isPalindrome("adam"));
        assertFalse(isPalindrome("12345"));
    }
}