import {Textarea} from "@nextui-org/react";
import {useState} from "react";
import {TauriClient} from "../tauri/TauriClient.ts";

import {debounce} from 'lodash';
import TextVO from "../pojo/vo/TextVO.ts";

export default function SearchBox() {

    const [text, setText] = useState('');
    const [searchResult, setSearchResult] = useState<TextVO[]>([]);
    return (
        <>
            <Textarea
                label="快速搜索"
                placeholder="输入关键字"
                className="flex w-full h-full"
                radius="none"
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
        </>
    )
}