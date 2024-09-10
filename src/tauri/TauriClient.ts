// When using the Tauri API npm package:
import {invoke} from '@tauri-apps/api/tauri'
import {Event, listen} from '@tauri-apps/api/event'

import React from "react";

export class TauriClient {
    static copyToClipboard(text: string) {
        invoke('copy_to_clipboard', {
            text: text
        })
            .then(r => console.log(r))
            .catch(e => console.error(e))
    }

    static pasteIntoCurrentWindow(text: string) {
        invoke('paste_into_current_window', {
            text: text
        })
            .then(r => console.log(r))
            .catch(e => console.error(e))
    }

    static registerShowSearchBoxListener(setShowSearchBox: React.Dispatch<React.SetStateAction<boolean>>) {
        listen('showSearchBox', (event: Event<boolean>) => {
            console.log(event)
            setShowSearchBox(event.payload)
        })
            .then(r => {
                console.log(r)
            })
            .catch(e => {
                console.error(e)
            })
    }
}