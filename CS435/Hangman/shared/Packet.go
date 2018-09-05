package shared

import (
	"bytes"
	"encoding/gob"
)

//PacketSend is just the data that the client and server will send to each other
type PacketSend struct {
	Header string
	Data   string
}

//psuedo enum for packet header types
const (
	ServerReady          string = "SERVER_READY"
	ServerLetterHit      string = "SERVER_LETTER_HIT"
	ServerLetterMiss     string = "SERVER_LETTER_MISS"
	ServerLetterDupl     string = "SERVER_LETTER_DUPL"
	ServerWordMiss       string = "SERVER_WORD_MISS"
	ServerGameEndSuccess string = "SERVER_GAME_END_SUCCESS"
	ServerGameEndFail    string = "SERVER_GAME_END_FAIL"
	ServerSendStatus     string = "SERVER_SEND_STATUS"

	ClientReady         string = "CLIENT_READY"
	ClientGuessLetter   string = "CLIENT_GUESS_LETTER"
	ClientGuessWord     string = "CLIENT_GUESS_WORD"
	ClientQuit          string = "CLIENT_QUIT"
	ClientRequestStatus string = "CLIENT_REQUEST_STATUS"
)

func Encode(header string, data string) *bytes.Buffer {

	binaryBuffer := new(bytes.Buffer)
	obj := gob.NewEncoder(binaryBuffer)
	obj.Encode(PacketSend{Header: header, Data: data})

	return binaryBuffer
}

func Decode(buffer []byte) *PacketSend {
	tmpBuffer := bytes.NewBuffer(buffer)
	tmpPacket := new(PacketSend)

	obj := gob.NewDecoder(tmpBuffer)
	obj.Decode(tmpPacket)

	return tmpPacket
}
