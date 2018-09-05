package main

import (
	"bufio"
	"fmt"
	"log"
	"net"
	"os"
	Packet "shared"
	"strconv"
	"strings"
)

var isPlaying = false

func main() {

	fmt.Print("Enter server to connect to: ")
	inputReader := bufio.NewReader(os.Stdin)
	addr, _ := inputReader.ReadString('\n')
	addr = strings.TrimSpace(addr)
	raddr, err := net.ResolveTCPAddr("tcp", addr)
	if err != nil {
		fmt.Println(err)
		os.Exit(1)
	}

	conn, err := net.DialTCP("tcp", nil, raddr)
	if err != nil {
		fmt.Println("failed to connect to server:", err)
		os.Exit(1)
	}

	conn.SetKeepAlive(true)

	tmpBufer := Packet.Encode(Packet.ClientReady, "Client is connected and ready")
	conn.Write(tmpBufer.Bytes())

	buf := make([]byte, 1024)

	_, err = conn.Read(buf)
	if err != nil {
		log.Panicln(err)
	}

	incPacket := Packet.Decode(buf)
	fmt.Println(">> " + incPacket.Header + " received from server.\n")
	fmt.Println(incPacket.Data + "\n\n")
	isPlaying = true

	for isPlaying {

		reader := bufio.NewReader(os.Stdin)
		//show menu options
		fmt.Println("-----Options-----")
		fmt.Println("1.) Guess a letter")
		fmt.Println("2.) Guess the word")
		fmt.Println("3.) View status")
		fmt.Println("4.) Quit")
		fmt.Print("\nEnter your choice: ")

		strChoice, _ := reader.ReadString('\n')

		intChoice, err := strconv.Atoi(strings.Replace(strChoice, "\r\n", "", -1))
		if err != nil {
			fmt.Print("Invalid input, try again.\n")
			continue
		}

		switch intChoice {
		case 1:
			fmt.Print("Guess a letter: ")
			letter, _ := reader.ReadString('\n')
			binBuffer := Packet.Encode(Packet.ClientGuessLetter, letter)
			conn.Write(binBuffer.Bytes())
			break
		case 2:
			fmt.Print("Guess the word: ")
			word, _ := reader.ReadString('\n')
			binBuffer := Packet.Encode(Packet.ClientGuessWord, word)
			conn.Write(binBuffer.Bytes())
			break
		case 3:
			binBuffer := Packet.Encode(Packet.ClientRequestStatus, "")
			conn.Write(binBuffer.Bytes())
		case 4:
			binBuffer := Packet.Encode(Packet.ClientQuit, "")
			conn.Write(binBuffer.Bytes())
			break
		default:
			binBuffer := Packet.Encode(Packet.ClientQuit, "")
			conn.Write(binBuffer.Bytes())
			break
		}

		buf := make([]byte, 1024)

		_, err = conn.Read(buf)
		if err != nil {
			log.Panicln(err)
		}

		incPacket := Packet.Decode(buf)
		fmt.Println(">> " + incPacket.Header + " received from server.\n")
		switch incPacket.Header {
		case Packet.ServerLetterHit:
			fmt.Println("You guessed correctly!")
			fmt.Println(incPacket.Data + "\n")
			break
		case Packet.ServerLetterMiss:
			fmt.Println(incPacket.Data)
			break
		case Packet.ServerGameEndFail:
			fmt.Println(incPacket.Data)
			conn.Close()
			isPlaying = false
			break
		case Packet.ServerGameEndSuccess:
			fmt.Println(incPacket.Data)
			conn.Close()
			isPlaying = false
			break
		case Packet.ServerSendStatus:
			fmt.Println(incPacket.Data + "\n")
			break
		case Packet.ServerLetterDupl:
			fmt.Println(incPacket.Data + "\n")
		}
	}

}
