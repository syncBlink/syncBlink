import styled from "styled-components";
import logo from '../assets/logo.png';

function DesktopHeader() {
    return (
        <StyledDesktopHeader>
            <a href="/" className="stretch"><img src={logo} alt="syncBlink Logo" /></a>          
        </StyledDesktopHeader>
    );
}

const StyledDesktopHeader = styled.div`
    background: white;
    border-radius: 5px;
    box-shadow: 0px 0px 5px rgba(0,0,0,0.1);
    
    position:absolute;
    display:flex;
    z-index:999;
    padding: ${ p => p.theme.contentPadding }px;
    margin: ${ p => p.theme.contentPadding }px;

    img {
        width: 100px;
        vertical-align: middle;
    }

    .stretch {        
        flex:1;
    }

    nav a{
        margin-left: 25px;
        font-weight: 700;
        font-size: 1.2rem;
        text-decoration: none;
    }

    nav a:hover {
        color: ${ p => p.theme.primaryColor };
    }

    nav .active {        
        color: ${ p => p.theme.textColor };
    }
`;

export default DesktopHeader;
