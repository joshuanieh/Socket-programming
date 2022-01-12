import './App.css'
import React, { useEffect, useRef, useState } from 'react'
import useChat from './useChat'
import { Button, Input, message, Tag } from 'antd'

function App() {
  const { status, opened, messages, sendMessage, clearMessages } = useChat()

  const [username, setUsername] = useState('')
  const [options, setOptions] = useState(false)
  const [chatRoom, setChatRoom] = useState(false)
  const [addFriend, setAddFriend] = useState(false)
  const [removeFriend, setRemoveFriend] = useState(false)
  const [listFriends, setListFriends] = useState(false)
  const [body, setBody] = useState('')

  const bodyRef = useRef(null)

  const displayStatus = (s) => {
    if (s.msg) {
      const { type, msg } = s
      const content = {
        content: msg,
        duration: 0.5
      }

      switch (type) {
        case 'success':
          message.success(content)
          break
        case 'info':
          message.info(content)
          break
        case 'danger':
        default:
          message.error(content)
          break
      }
    }
  }

  useEffect(() => {
    displayStatus(status)
  }, [status])

  return (
    <div className="App">
      {!options ?
        <div> 
          <div className="App-title">
            <h1>Login</h1>
          </div>
          <Input.Search
            placeholder="Please enter a username:"
            enterButton="Continue"
            ref={bodyRef}
            style={{ marginBottom: 10 }}
            onChange={(e) => setUsername(e.target.value)}
            onSearch={(msg) => {
              !msg ? 
                displayStatus({
                  type: 'error',
                  msg: 'Please enter a username.'
                }) 
                : 
                sendMessage(`Login {username}`)
                setOptions(true)
            }}>
            </Input.Search>
        </div> 
        : 
        <>
        {listFriends ?
          <div>
            <div className="App-title">
              <h1>All friends</h1>
            </div>
            <div className="App-messages">
              {messages.length === 0 ? (
                <p style={{ color: '#ccc' }}>
                  {opened? 'No messages...' : 'Loading...'}
                </p>
              ) : (
                <p className="App-message">
                  <Tag color="blue">{messages}</Tag>
                </p>
              )}
            </div>
            <Button type="primary" danger onClick={() => {
              setListFriends(false)
            }}>
              Leave
            </Button>
           </div>
          : 
          <>
            <div className="App-title">
              <h1>Options</h1>
            </div>
            <Button type="primary" style={{margin: '20px'}} danger onClick={() => {
              sendMessage(`List friends`)
              setListFriends(true)
            }}>
              List all friends
            </Button>
            <Button type="primary" style={{margin: '20px'}} danger onClick={() => {
              setAddFriend(true)
            }}>
              Add a friend
            </Button>
            <Button type="primary" style={{margin: '20px'}} danger onClick={() => {
              sendMessage(`List friends`)
              setRemoveFriend(true)
            }}>
              Remove a friend
            </Button>
            <Button type="primary" style={{margin: '20px'}} danger onClick={() => {
              setChatRoom(true)
            }}>
              Chat with
            </Button>
          </>
        }
        </>
      }
    </div>
  )

//   return (
//     <div className="App">
      // <div className="App-title">
      //   <h1>Simple Chat</h1>
      //   <Button type="primary" danger onClick={clearMessages}>
      //     Clear
      //   </Button>
      // </div>
      // <div className="App-messages">
      //   {messages.length === 0 ? (
      //     <p style={{ color: '#ccc' }}>
      //       {opened? 'No messages...' : 'Loading...'}
      //     </p>
      //   ) : (
      //     messages.map(({ name, body }, i) => (
      //       <p className="App-message" key={i}>
      //         <Tag color="blue">{name}</Tag> {body}
      //       </p>
      //     ))
      //   )}
      // </div>
//       <Input
//         placeholder="Username"
//         value={username}
//         onChange={(e) => setUsername(e.target.value)}
//         style={{ marginBottom: 10 }}
//         onKeyDown={(e) => {
//           if (e.key === 'Enter') {
//             bodyRef.current.focus()
//           }
//         }}
//       ></Input>
//       <Input.Search
//         rows={4}
//         value={body}
//         ref={bodyRef}
//         enterButton="Send"
//         onChange={(e) => setBody(e.target.value)}
//         placeholder="Type a message here..."
//         onSearch={(msg) => {
//           if (!msg || !username) {
//             displayStatus({
//               type: 'error',
//               msg: 'Please enter a username and a message body.'
//             })
//             return
//           }

//           sendMessage({ name: username, body: msg })
//           setBody('')
//         }}
//       ></Input.Search>
//     </div>
//   )
}

export default App
