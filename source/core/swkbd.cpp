#include "common.h"

std::string getKeyboardInput(Text myText, const char *hintText, u32 maxLength, SwkbdType keyboardType) {
	SwkbdState swkbdState;
	SwkbdButton button = SWKBD_BUTTON_NONE;
	swkbdInit(&swkbdState, keyboardType, 2, maxLength);
	swkbdSetHintText(&swkbdState, hintText);
	swkbdSetInitialText(&swkbdState, myText.GetText().c_str());

	char *outputBuffer = new char[maxLength + 1];
	button = swkbdInputText(&swkbdState, outputBuffer, maxLength + 1);
	std::string str;

	if (button == SWKBD_BUTTON_CONFIRM) {
		str = std::string(outputBuffer);
	}
	else {
		str = std::string(" ");
	}

	delete[] outputBuffer;
	return str;
}

void DictInit(SwkbdState* swkbd)
{
    SwkbdDictWord words[2];
    swkbdSetDictWord(&words[0], "lenny", "( ͡° ͜ʖ ͡°)");
    swkbdSetDictWord(&words[1], "shrug", "¯\\_(ツ)_/¯");
    swkbdSetDictionary(swkbd, words, sizeof(words)/sizeof(SwkbdDictWord));
}