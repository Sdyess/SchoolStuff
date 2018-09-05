package main

import (
	Packet "Hangman/shared"
	"fmt"
	"math/rand"
	"net"
	"os"
	"strconv"
	"strings"
	"unicode"
)

type Game struct {
	word         string
	guessedChars []string
	guessed      string
	misses       int32
}

var currentGame Game

func main() {

	listener, err := net.Listen("tcp", ":4040")
	if err != nil {
		fmt.Println(err)
		os.Exit(1)
	}

	defer listener.Close()

	fmt.Println("Server is ready...")

	for {
		conn, err := listener.Accept()
		if err != nil {
			fmt.Println(err)
			conn.Close()
			continue
		}

		go handleRequest(conn)
	}
}

func handleRequest(conn net.Conn) {
	defer conn.Close()

	for {
		buffer := make([]byte, 1024)

		_, err := conn.Read(buffer)
		if err != nil {
			fmt.Println(err)
			return
		}

		incPacket := Packet.Decode(buffer)

		switch incPacket.Header {
		case Packet.ClientReady:
			packetReceived(incPacket.Header, conn)
			startGame(false)
			sendResponse(Packet.ServerReady, currentGame.guessed, conn)
			break
		case Packet.ClientGuessLetter:
			packetReceived(incPacket.Header, conn)
			exists, count := checkLetterGuess(incPacket.Data, conn)
			if exists {
				if isGameOver() {
					sendResponse(Packet.ServerGameEndSuccess, "You've figured out the word! You win!\n"+currentGame.guessed, conn)
					startGame(true)
				} else {
					sendResponse(Packet.ServerLetterHit, "There is "+strconv.Itoa(count)+" matching letter(s).\nCurrent Results: "+currentGame.guessed, conn)
				}

			} else {
				if count < 0 {
					sendResponse(Packet.ServerLetterDupl, "You've already guessed that letter.\nCurrent Results: "+currentGame.guessed, conn)
				} else {
					currentGame.misses++
					sendResponse(Packet.ServerLetterMiss, "You've guessed incorrectly!\n"+showHangman()+"\nCurrent Results: "+currentGame.guessed, conn)
				}
			}
			break
		case Packet.ClientGuessWord:
			packetReceived(incPacket.Header, conn)
			if checkWordGuess(incPacket.Data) {
				sendResponse(Packet.ServerGameEndSuccess, "Congratulations! You guessed the word correctly!\n", conn)
			} else {
				currentGame.misses++
				sendResponse(Packet.ServerWordMiss, "You've guessed incorrectly!\n"+showHangman()+"\nCurrent Results: "+currentGame.guessed, conn)
			}
			break
		case Packet.ClientRequestStatus:
			packetReceived(incPacket.Header, conn)
			sendResponse(Packet.ServerSendStatus, showHangman()+"\nCurrent Results: "+currentGame.guessed, conn)
			break
		case Packet.ClientQuit:
			packetReceived(incPacket.Header, conn)
			sendResponse(Packet.ServerGameEndFail, "Thanks for playing!\n", conn)
			conn.Close()
			startGame(true)
		default:
			break
		}

	}
}

func sendResponse(header string, res string, conn net.Conn) {
	packetSent(header, conn)
	binBuffer := Packet.Encode(header, res)
	conn.Write(binBuffer.Bytes())
}

func packetReceived(packet string, c net.Conn) {
	fmt.Println(">> " + packet + " received from client: " + c.LocalAddr().String())
}

func packetSent(packet string, c net.Conn) {
	fmt.Println(">> " + packet + " sent to client: " + c.LocalAddr().String())
}

func checkLetterGuess(letter string, c net.Conn) (bool, int) {
	if strings.Contains(currentGame.word, strings.TrimSpace(letter)) {
		if hasLetterBeenGuessed(letter) == false {
			updateWordChart(letter)
			currentGame.guessedChars = append(currentGame.guessedChars, letter)
			tmpArr := []rune(letter)
			tmpLetter := tmpArr[0]
			counter := 0

			for _, element := range currentGame.word {
				if element == tmpLetter {
					counter++
				}
			}
			return true, counter
		} else {
			return false, -1
		}
	}

	return false, 0
}

func hasLetterBeenGuessed(letter string) bool {
	for _, element := range currentGame.guessedChars {
		if element == letter {
			return true
		}
	}
	return false
}

func updateWordChart(letter string) {
	dashNum := 0
	for index, element := range currentGame.guessed {

		if element == '_' {
			runeLetter := []rune(letter)[0]
			runeWord := []rune(currentGame.word)

			if runeWord[dashNum] == runeLetter {
				oldGuess := []rune(currentGame.guessed)
				oldGuess[index] = runeLetter

				currentGame.guessed = string(oldGuess)
			}
			dashNum++
		} else if unicode.IsLetter(element) {
			dashNum++
		}

	}
}

func checkWordGuess(guess string) bool {
	if strings.TrimSpace(guess) == currentGame.word {
		return true
	}

	return false
}

func showHangman() string {
	switch currentGame.misses {
	case 1:
		return "  +---+\n  |   |\n      |\n      |\n      |\n      |\n=======\n"
	case 2:
		return "  +---+\n  |   |\n  O   |\n      |\n      |\n      |\n=======\n"
	case 3:
		return "  +---+\n  |   |\n  O   |\n  |   |\n      |\n      |\n=======\n"
	case 4:
		return "  +---+\n  |   |\n  O   |\n /|   |\n      |\n      |\n=======\n"
	case 5:
		return "  +---+\n  |   |\n  O   |\n /|\\  |\n      |\n      |\n=======\n"
	case 6:
		return "  +---+\n  |   |\n  O   |\n /|\\  |\n /    |\n      |\n=======\n"
	case 7:
		return "  +---+\n  |   |\n  O   |\n /|\\  |\n / \\  |\n      |\n=======\n"
	}

	return ""
}

func isGameOver() bool {
	if strings.Contains(currentGame.guessed, "_") {
		return false
	}
	return true
}

func startGame(forced bool) {
	if currentGame.word == "" || forced {
		selectedWord := selectWord()
		currentGame = Game{word: selectedWord, guessedChars: nil, guessed: BuildBlanks(selectedWord)}
	}

}

func selectWord() string {
	words := [7]string{"hello", "networking", "aardvark", "faux", "genre", "vellum", "triangulation"}
	return words[rand.Intn(len(words))]
}

func BuildBlanks(word string) string {

	tmp := ""

	for i := 0; i < len(word); i++ {
		tmp = tmp + "_ "
	}

	return tmp[0 : len(tmp)-1]
}
