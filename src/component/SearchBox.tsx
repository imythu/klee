import {Input} from "@nextui-org/react";
import {useState} from "react";
import {TauriClient} from "../tauri/TauriClient.ts";

import {debounce} from 'lodash';
import TextVO from "../pojo/vo/TextVO.ts";

export default function SearchBox() {

    const [text, setText] = useState('');
    const [searchResult, setSearchResult] = useState<TextVO[]>([]);
    return (
        <div
            className={'w-[48rem] h-[6rem] px-8 rounded-0 flex justify-center items-center ' +
                'bg-gradient-to-tr from-pink-500 to-yellow-500 text-white shadow-lg' +
                ''}>
            <Input
                label="Search"
                isClearable
                radius="lg"
                classNames={{
                    label: "text-black/50 dark:text-white/90",
                    input: [
                        "bg-transparent",
                        "text-black/90 dark:text-white/90",
                        "placeholder:text-default-700/50 dark:placeholder:text-white/60",
                    ],
                    innerWrapper: "bg-transparent",
                    inputWrapper: [
                        "shadow-xl",
                        "bg-default-200/50",
                        "dark:bg-default/60",
                        "backdrop-blur-xl",
                        "backdrop-saturate-200",
                        "hover:bg-default-200/70",
                        "dark:hover:bg-default/70",
                        "group-data-[focus=true]:bg-default-200/50",
                        "dark:group-data-[focus=true]:bg-default/60",
                        "!cursor-text",
                    ],
                }}
                placeholder="Type to search..."
                startContent={
                    <svg
                        aria-hidden="true"
                        fill="none"
                        focusable="false"
                        height="1em"
                        role="presentation"
                        viewBox="0 0 24 24"
                        width="1em"
                        className="text-black/50 mb-0.5 dark:text-white/90 text-slate-400 pointer-events-none flex-shrink-0"
                    >
                        <path
                            d="M11.5 21C16.7467 21 21 16.7467 21 11.5C21 6.25329 16.7467 2 11.5 2C6.25329 2 2 6.25329 2 11.5C2 16.7467 6.25329 21 11.5 21Z"
                            stroke="currentColor"
                            strokeLinecap="round"
                            strokeLinejoin="round"
                            strokeWidth="2"
                        />
                        <path
                            d="M22 22L20 20"
                            stroke="currentColor"
                            strokeLinecap="round"
                            strokeLinejoin="round"
                            strokeWidth="2"
                        />
                    </svg>
                }
                value={text}
                onChange={e => {
                    setText(e.target.value);
                    debounce(() => {
                        TauriClient.search(text).then(r => {
                            console.log(r);
                            if (r.data && r.data.length > 0) {
                                setSearchResult(r.data);
                            }
                        })
                            .catch(e => console.error(e))
                    }, 500)()
                }
                }
            />
            {
                searchResult && searchResult.length > 0 && (
                    <div>
                        result
                    </div>
                )
            }
        </div>
    )
}