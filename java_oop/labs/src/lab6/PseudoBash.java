/*
 * Разработать консольное приложение, позволяющее просматривать файлы и каталоги файловой системы, а также создавать и
 * удалять текстовые файлы. Для работы с текстовыми файлами необходимо реализовать функциональность записи (дозаписи) в
 * файл.
 */

package lab6;

import java.io.*;
import java.util.Objects;
import java.util.Scanner;

public class PseudoBash
{
    private static File current_dir = new File(System.getProperty("user.dir")).getAbsoluteFile();

    public static void help()
    {
        System.out.println("+========================= HELP ==========================+");
        System.out.println("| ls                         List information about files |" );
        System.out.println("| cd [Directory]             Change Directory             |" );
        System.out.println("| touch [Filename]           Create text file             |" );
        System.out.println("| rm [Filename]              Delete text file             |" );
        System.out.println("| write [Filename] [Text]    Write text to file           |" );
        System.out.println("| append [Filename] [Text]   Write/Rewrite text to file   |" );
        System.out.println("| cat [Filename]             Print text from file         |" );
        System.out.println("| exit                       Stop PseudoBash              |" );
        System.out.println("+=========================================================+\n" );
    }

    public static void ls()
    {
        try
        {
            for (File file : Objects.requireNonNull(current_dir.listFiles()))
                System.out.println(file.getName());
        }
        catch (NullPointerException error)
        {
            System.out.println("Directory " + current_dir.getName() + " is not exist!");
        }
    }

    public static void cd(String path)
    {
        var temp_path = current_dir.getAbsolutePath();
        current_dir = new File(temp_path + "/" + path).getAbsoluteFile();

        if (!current_dir.exists())
        {
            System.out.println("Directory " + current_dir.getAbsolutePath() + " is not exist!");
            current_dir = new File(temp_path).getAbsoluteFile();
        }
    }

    public static void touch(String file_name)
    {
        try
        {
            File text_file = new File(current_dir.getAbsolutePath() + "/" + file_name);

            if (text_file.createNewFile())
                System.out.println("File " + file_name + " created.");
            else
                System.out.println("File " + file_name + " already exists.");
        }
        catch (IOException error)
        {
            System.out.println("Program trow exception: " + error.toString());
        }
    }

    public static void rm(String file_name)
    {
        try
        {
            File text_file = new File(current_dir.getAbsolutePath() + "/" + file_name);

            if (text_file.delete())
                System.out.println("File " + file_name + " deleted.");
             else
                System.out.println("File " + file_name + " is not exists.");
        }
        catch (Exception error)
        {
            System.out.println("Program trow exception: " + error.toString());
        }
    }

    public static void write(String file_name, String text, boolean is_append)
    {
        try (BufferedWriter buff_writer = new BufferedWriter(new FileWriter(current_dir + "/" + file_name, is_append)))
        {
            buff_writer.write(text);
            buff_writer.newLine();
            buff_writer.flush();
            if (is_append)
                System.out.println("Append text in " + file_name);
             else
                System.out.println("Write text to " + file_name);
        }
        catch (IOException error)
        {
            System.out.println("Program trow exception: " + error.toString());
        }
    }

    public static void cat(String file_name)
    {
        try (BufferedReader buff_reader = new BufferedReader(new FileReader(current_dir + "/" + file_name)))
        {
            String line;
            while ((line = buff_reader.readLine()) != null)
            {
                System.out.println(line);
            }
        }
        catch (IOException error)
        {
            System.out.println("Program trow exception: " + error.toString());
        }
    }

    public static void exit()
    {
        System.out.println("Good bye!");
        System.exit(0);
    }

    public static String getText(String[] argv)
    {
        StringBuilder text = new StringBuilder();

        for (int i = 2; i < argv.length; ++i)
            text.append(argv[i] + " ");

        text.append("\n");
        return text.toString();
    }

    public static void main(String[] args)
    {
        System.out.println("Hello! Use bash commands for manipulate program. Print \"help\" to see all available commands\n");
        System.out.print('>');
        try (Scanner input = new Scanner(System.in))
        {
            String command_line;

            while (!(command_line = input.nextLine()).isEmpty())
            {
                String [] argv = command_line.split(" ");

                switch (argv[0])
                {
                    case "ls" :
                    {
                        ls();
                        break;
                    }
                    case "cd" :
                    {
                        cd(argv[1]);
                        break;
                    }
                    case "help" :
                    {
                        help();
                        break;
                    }
                    case "touch" :
                    {
                        touch(argv[1]);
                        break;
                    }
                    case "rm" :
                    {
                        rm(argv[1]);
                        break;
                    }
                    case "cat" :
                    {
                        cat(argv[1]);
                        break;
                    }
                    case "write" :
                    {
                        write(argv[1], getText(argv), false);
                        break;
                    }
                    case "append" :
                    {
                        write(argv[1], getText(argv), true);
                        break;
                    }
                    case "exit" :
                    {
                        exit();
                        break;
                    }
                    default :
                    {
                        System.out.println("Wrong command!");
                        help();
                        break;
                    }
                }

                System.out.print('>');
            }
        }
        catch (Exception error)
        {
            System.out.println("Program throw exception " + error.toString());
        }
    }
}