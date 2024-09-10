import {Textarea} from "@nextui-org/react";

export default function SearchBox() {
    return (
        <Textarea
            label="快速搜索"
            placeholder="输入关键字"
            className="max-w-xs"
        />
    )
}