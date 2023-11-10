package client;

import java.net.*;
import java.io.*;

public class Client {
    private Socket           socket    = null;

    private DataInputStream  console    = null;
    private DataInputStream  fromServer = null;

    private DataOutputStream streamOut = null;

    public Client(String serverName, int serverPort) {
        System.out.println("Connect with AliceFalloutServer. Please wait ...");

        try {
            socket = new Socket(serverName, serverPort);
            System.out.println("Connected: " + socket);
            start();
        } catch(UnknownHostException e) {
            System.out.println("Host unknown: " + e.getMessage());
        } catch (IOException e) {
            System.out.println(e);
        }

        String line = "";
        while (!line.equals("close")) {
            try {
                line = console.readLine();
                streamOut.writeUTF(line);
                streamOut.flush();

                String alice_answer = fromServer.readUTF();
                System.out.println("AliceFalloutServer: " + alice_answer);

                if (line.equals("close"))
                    stop();
            } catch (IOException e) {
                System.out.println("Sending error: " + e.getMessage());
            }
        }
    }

    public void start() throws IOException {
        console   = new DataInputStream(System.in);
        fromServer= new DataInputStream(new BufferedInputStream(socket.getInputStream()));
        streamOut = new DataOutputStream(socket.getOutputStream());
    }

    public void stop() throws IOException {
        try {
            if (console != null)
                console.close();

            if (streamOut != null)
                streamOut.close();

            if (socket != null)
                socket.close();
        } catch (IOException e) {
            System.out.println("Error closing ...");
        }
    }

    public static void main(String[] args) {
        Client client = null;

        if (args.length != 2)
            System.out.println("Need server name and port!");
        else
            client = new Client(args[0], Integer.parseInt(args[1]));
    }
}