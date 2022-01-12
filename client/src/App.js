import './App.css'

import React, { useEffect, useRef, useState } from 'react'
// import useChat from './useChat'
import { Button, Input, message, Tag } from 'antd'

function App() {
  // const { status, opened, messages, sendMessage, clearMessages } = useChat()
  
  // const http = new XMLHttpRequest()
  // http.open("POST", "https://localhost:4000", true)
  // http.setRequestHeader('Content-Type', 'text/plain;charset=UTF-8')
  var http = require('http');
  // var agent = new http.Agent({ keepAlive: true }); // false by default
  
  

  // http.request({
  //     host: '127.0.0.1',
  //     port: 4000,
  //     method: 'POST',
  //     agent: agent, // use this agent for more requests as needed
  //     data: 'lll'
  // }).end();
  const [messages, setMessages] = useState("")
  const [textFinish, setTextFinish] = useState("")
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

  // useEffect(() => {
  //   displayStatus(status)
  // }, [status])

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
              if(msg) {
                  ////////////////////////// sendMessage(`Login {username}`)
                  const da = `Login ${msg}`
                  const option = {
                    hostname: '127.0.0.1',
                    port: 4000,
                    method: 'POST',
                    headers: {
                      'Content-Type': 'text/plain',
                      'Content-Length': da.length,
                      'Sec-Fetch-Mode': 'no-cors'
                    }
                  }

                  const req = http.request(option, res => {
                    console.log(`statusCode: ${res.statusCode}`)

                    res.on('data', d => {
                      process.stdout.write(d)
                    })
                  })

                  req.on('error', error => {
                    console.error(error)
                  })

                  req.write(da)
                  req.end()
                  setOptions(true)
              }
              else {
                displayStatus({
                  type: 'error',
                  msg: 'Please enter a username.'
                })
              } 
            }}>
            </Input.Search>
        </div> 
        : 
        <>
        {listFriends ?
          <>
            <div className="App-title">
              <h1>All friends</h1>
            </div>
            <div>
              <p className="App-message">
                <Tag color="blue">{messages}</Tag>
              </p>
            </div>
            <Button type="primary" danger onClick={() => {
              setListFriends(false)
            }}>
              Leave
            </Button>
          </>
          : 
          <>
            {addFriend ? 
              <>
                <div className="App-title">
                  <h1>Add friends</h1>
                </div>
                <Input.Search
                  placeholder="Please enter a username:"
                  enterButton="Add"
                  ref={bodyRef}
                  style={{ marginBottom: 10 }}
                  onChange={(e) => setUsername(e.target.value)}
                  onSearch={(msg) => {
                    if(msg) {
                        ////////////////////// sendMessage(`Add {username}`)
                    }
                    else {
                      displayStatus({
                        type: 'error',
                        msg: 'Please enter a username.'
                      })
                    } 
                  }}>
                </Input.Search>
                <Button type="primary" danger onClick={() => {
                  setAddFriend(false)
                }}>
                  Leave
                </Button>
              </>
              :
              <>
                {removeFriend ? 
                  <>
                    <div className="App-title">
                      <h1>Remove friends</h1>
                    </div>
                    <Input.Search
                      placeholder="Please enter a username:"
                      enterButton="Remove"
                      ref={bodyRef}
                      style={{ marginBottom: 10 }}
                      onChange={(e) => setUsername(e.target.value)}
                      onSearch={(msg) => {
                        if(msg) {
                            //////////////////sendMessage(`Remove {username}`)
                        }
                        else {
                          displayStatus({
                            type: 'error',
                            msg: 'Please enter a username.'
                          })
                        }
                      }}>
                    </Input.Search>
                    <Button type="primary" danger onClick={() => {
                      setRemoveFriend(false)
                    }}>
                      Leave
                    </Button>
                  </>
                  :
                  <>
                    {chatRoom ? 
                      <>
                        <div className="App-title">
                          <h1>Chat room</h1>
                        </div>
                        <div>
                          <p className="App-message">
                            <Tag color="blue">{messages}</Tag>
                          </p>
                        </div>
                        <Button type="primary" danger onClick={() => {
                          setChatRoom(false)
                        }}>
                          Leave
                        </Button>
                      </>
                      :
                      <>
                        <div className="App-title">
                          <h1>Options</h1>
                        </div>
                        <Button type="primary" style={{margin: '20px'}} danger onClick={() => {
                          /////////////////////sendMessage('List friends')
                          // http.onload = () => console.log(http.responseText)
                          // http.send('List friends')
                          //////////////////////////
                          // const da = JSON.stringify({
                          //   todo: 'Buy the milk'
                          // })
                          const da = "List friends"
                          const option = {
                            hostname: '127.0.0.1',
                            port: 4000,
                            method: 'POST',
                            headers: {
                              'Content-Type': 'text/plain',
                              'Content-Length': da.length,
                              'Sec-Fetch-Mode': 'no-cors'
                            }
                          }

                          const req = http.request(option, res => {
                            console.log(`statusCode: ${res.statusCode}`)

                            res.on('data', d => {
                              process.stdout.write(d)
                            })
                          })

                          req.on('error', error => {
                            console.error(error)
                          })

                          req.write(da)
                          req.end()
                          /////////////////////////////
                          setListFriends(true)
                        }}>
                          List all friends
                        </Button>
                        <Button type="primary" style={{margin: '20px'}} danger onClick={() => {
                          setAddFriend(true)
                        }}>
                          Add friends
                        </Button>
                        <Button type="primary" style={{margin: '20px'}} danger onClick={() => {
                          // sendMessage(`List friends`)
                          setRemoveFriend(true)
                        }}>
                          Remove friends
                        </Button>
                        <Button type="primary" style={{margin: '20px'}} danger onClick={() => {
                          //////////////sendMessage('List friends')
                          setChatRoom(true)
                        }}>
                          Chat with
                        </Button>
                      </>
                    }
                  </>
                }
              </>
            }
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
