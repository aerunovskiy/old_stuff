/**
 * Напишите функцию boolean isPalindrome(String input), проверяющую, является ли строка полиндромом. Протестируйте.
 */
package lab23;

public class Palindrome {
    public static boolean isPalindrome(String input) {
        String clean = input.replaceAll("[^a-zA-Z0-9]", "");
        StringBuilder sb = new StringBuilder(clean);
        return clean.equalsIgnoreCase(sb.reverse().toString());
    }
}
