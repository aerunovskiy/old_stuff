package com.alice;

import java.net.*;
import java.io.*;

public class AliceFalloutServer {
    private Socket socket              = null;
    private ServerSocket server        = null;
    private DataInputStream streamIn   = null;
    private DataOutputStream streamOut = null;

    public AliceFalloutServer(int port) {
        try {
            System.out.println("AliceFalloutServer binding to port " + port + " please wait ...");
            server = new ServerSocket(port);
            System.out.println("AliceFalloutServer waiting for a client ...");
            socket = server.accept();
            System.out.println("Client accepted:" + socket);
            open();
            boolean done = false;
            while (!done) {
                try {
                    String line = streamIn.readUTF();
                    System.out.println("Receive message: " + line);
                    done = line.equals("close");

                    String answer =  (!done) ? (getRandomWord(line) + "?") : "Good bye!";

                    streamOut.writeUTF(answer);
                    streamOut.flush();
                } catch (IOException e) {
                    done = true;
                }
            }

            close();
        } catch (IOException e) {
            System.out.println(e);
        }
    }

    public void open() throws IOException {
        streamIn = new DataInputStream(new BufferedInputStream(socket.getInputStream()));
        streamOut = new DataOutputStream(socket.getOutputStream());
    }

    public void close() throws IOException {
        if (socket != null)
            socket.close();

        if(streamIn != null)
            streamIn.close();
    }

    public String getRandomWord (String messageFromClient) {
        String[] words = messageFromClient.split("\\W");
        return words[getRandomNumber(0, words.length - 1)];
    }

    public int getRandomNumber(int min, int max) {
        return (int) ((Math.random() * (max - min)) + min);
    }

    public static void main(String[] args) {
        AliceFalloutServer aliceFalloutServer = null;

        if (args.length != 1)
            System.out.println("Need port only!");
        else
            aliceFalloutServer = new AliceFalloutServer(Integer.parseInt(args[0]));
    }
}