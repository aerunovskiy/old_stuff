/*
 * Дана строка. Разделить строку на фрагменты по три подряд идущих символа. В каждом фрагменте средний символ
 * заменить на случайный символ, не совпадающий ни с одним из символов этого фрагмента. Вывести в консоль фрагменты,
 * упорядоченные по алфавиту.
 */

package lab3;

import java.util.Arrays;
import java.util.Random;

public class StringEditor
{
    static void editString(String[] strings)
    {
        for (int i = 0; i < strings.length; ++i)
            strings[i] = replaceSymbol(strings[i]);
        System.out.println();
    }

    static char getRandomChar()
    {
        Random random = new Random();
        return (char)(48 + random.nextInt(47));
    }

    static String replaceSymbol(String str)
    {
        char[] chars    = str.toCharArray();
        char randomChar = getRandomChar();
        while (str.contains(String.valueOf(randomChar)))
            randomChar = getRandomChar();

        chars[chars.length / 2] = randomChar;
        return new String(chars);
    }

    public static void main(String[] args)
    {
        String str = "Lorem ipsum dolor sit amet, consectetur adipiscing elit";
        String[] trinities = str.split("(?<=\\G.{3})");
        editString (trinities);
        Arrays.sort(trinities);
        System.out.println(Arrays.toString(trinities));
    }
}