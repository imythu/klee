import {Route, Routes, useNavigate} from "react-router-dom";
import {NextUIProvider} from "@nextui-org/react";
import SearchBox from "./component/SearchBox.tsx";
import HomePage from "./component/HomePage.tsx";

export default function App() {
    const navigate = useNavigate();
    return (

        <NextUIProvider navigate={navigate}>
            <Routes>
                <Route path="/" element={<HomePage />} />
                <Route path="/search-box" element={<SearchBox />} />
            </Routes>
        </NextUIProvider>
    );
}
