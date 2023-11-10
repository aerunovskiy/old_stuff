/*
 * Пользователь вводит некоторое число. Записать его цифры в стек. Вывести число, у которого цифры идут в обратном
 * порядке. Предусмотреть возможность введения произвольного количества чисел.
 */

package lab5;

import java.util.ArrayList;
import java.util.Scanner;
import java.util.Stack;

    public class StackNumbers
    {
        public static ArrayList<Integer> userInput()
        {
            ArrayList<Integer> number_array = new ArrayList<>();
            Scanner in                      = new Scanner(System.in);
            boolean input_is_active         = true;

            System.out.println("Enter the numbers. 0 or wrong data are complete input:");

            try
            {
                while (input_is_active)
                {
                    int number = in.nextInt();

                    if (number != 0)
                        number_array.add(number);
                    else
                        input_is_active = false;

                }
        }
        catch (Exception error)
        {
            System.out.println("Program is throw exception "+error.toString());

            if (number_array.isEmpty())
            {
                System.out.println("Nothing to process! Stop program.");
                System.exit(0);
            }

            System.out.println("Forced stop numbers entering!");
        }

        return number_array;
    }

    public static Stack[] numbersToStackArray(ArrayList<Integer> numbers)
    {
        Stack[] stack_array = new Stack[numbers.size()];

        for (int i = 0; i < stack_array.length; ++i)
            stack_array[i] = numberToStack(numbers.get(i));

        return stack_array;
    }

    public static Stack numberToStack(Integer number)
    {
        Stack stack        = new Stack();
        char[] num_in_char = number.toString().toCharArray();

        for (int i = 0; i < num_in_char.length; ++i)
            stack.push(num_in_char[i]);

        return stack;
    }

    public static void printStackArray(Stack[] stack_array)
    {
        System.out.println("Numbers from stack:");

        System.out.println("\n+------+");

        for (Stack stack : stack_array)
        {
            while (!stack.isEmpty())
                System.out.print(stack.pop());

            System.out.println("\n+------+");
        }
    }

    public static void main(String[] args)
    {
        //====== INPUT NUMBERS ======
        var numbers = userInput();
        //====== NUMBERS TO STACK ======
        var numbers_in_stack = numbersToStackArray(numbers);
        //====== PRINT NUMBERS FROM STACK ======
        printStackArray(numbers_in_stack);
    }
}