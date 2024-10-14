// When using the Tauri API npm package:
import {invoke} from '@tauri-apps/api/core'
import {Event, listen} from '@tauri-apps/api/event'

import React from "react";
import TextVO from "../pojo/vo/TextVO.ts";
import ApiResponse from "../pojo/vo/ApiResponse.ts";
import {TextBO} from "../pojo/bo/TextBO.ts";

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

        static search(text: string): Promise<ApiResponse<TextVO[]>> {
        return invoke<ApiResponse<TextVO[]>>('search', {
            text: text
        });
    }

    static addText(addText: TextBO): Promise<ApiResponse<TextVO>> {
        return invoke<ApiResponse<TextVO>>('add_text', addText);
    }

    static deleteText(id: number): Promise<ApiResponse<boolean>> {
        return invoke<ApiResponse<boolean>>('delete_text', {
            id: id
        });
    }

    static updateText(text: TextBO): Promise<ApiResponse<TextVO>> {
        return invoke<ApiResponse<TextVO>>('update_text', text);
    }
}