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
  const [id, setID] = useState(0)
  const [status, setStatus] = useState({})
  const [messages, setMessages] = useState("")
  const [messagesList, setMessagesList] = useState([])
  const [friends, setFriends] = useState([])
  const [textFinish, setTextFinish] = useState("")
  const [username, setUsername] = useState('')
  const [options, setOptions] = useState(false)
  const [chatRoom, setChatRoom] = useState(false)
  const [chatting, setChatting] = useState(false)
  const [chattingFriend, setChattingFriend] = useState("")
  const [addFriend, setAddFriend] = useState(false)
  const [removeFriend, setRemoveFriend] = useState(false)
  const [listFriends, setListFriends] = useState(false)
  const [body, setBody] = useState('')

  const bodyRef = useRef(null)

  let friendString = ""
  let messageString = ""

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
                    }
                  }

                  const req = http.request(option, res => {
                    console.log(res)
                    console.log(`statusCode: ${res.statusCode}`)

                    res.on('data', d => {
                      console.log(d)
                      setID(d);
                    })
                  })
                  console.log(req)

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
              {friends.map((e, i) => (
                <p className="App-message" key={i}>
                  <Tag color="blue">{e}</Tag>
                </p>
              ))}
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
                        const da = `Add ${msg} ${id}`
                        const option = {
                          hostname: '127.0.0.1',
                          port: 4000,
                          method: 'POST',
                          headers: {
                            'Content-Type': 'text/plain',
                            'Content-Length': da.length
                          }
                        }

                        const req = http.request(option, res => {
                          console.log(`statusCode: ${res.statusCode}`)

                          res.on('data', d => {
                            console.log(d)
                            if(d[0] === 49){
                              displayStatus({
                                type: 'error',
                                msg: 'No user found.'
                              })
                            }
                            else if(d[0] === 48){
                              displayStatus({
                                type: 'success',
                                msg: 'Add user successfully'
                              })
                            }
                          })
                        })

                        req.on('error', error => {
                          console.error(error)
                        })

                        req.write(da)
                        req.end()
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
                    <div>
                      {friends.map((e, i) => (
                        <p className="App-message" key={i}>
                          <Button type="ghost" onClick={() => {
                            const da = `Remove ${e} ${id}`
                            const option = {
                              hostname: '127.0.0.1',
                              port: 4000,
                              method: 'POST',
                              headers: {
                                'Content-Type': 'text/plain',
                                'Content-Length': da.length
                              }
                            }
    
                            const req = http.request(option, res => {
                              console.log(`statusCode: ${res.statusCode}`)
    
                              res.on('data', d => {
                                console.log(d)
                                displayStatus({
                                  type: 'success',
                                  msg: 'Remove friend successfully.'
                                })
                                const index = friends.indexOf(e);
                                const copy = [...friends]
                                copy.splice(index, 1)
                                setFriends(copy)
                              })
                            })
    
                            req.on('error', error => {
                              console.error(error)
                            })
    
                            req.write(da)
                            req.end()
                          }}>{e}</Button>
                        </p>
                      ))}
                    </div>
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
                          {friends.map((e, i) => (
                            <p className="App-message" key={i}>
                              <Button type="ghost" onClick={() => {
                                const da = `Chat ${e} ${id}`
                                const option = {
                                  hostname: '127.0.0.1',
                                  port: 4000,
                                  method: 'POST',
                                  headers: {
                                    'Content-Type': 'text/plain',
                                    'Content-Length': da.length
                                  }
                                }
        
                                const req = http.request(option, res => {
                                  console.log(`statusCode: ${res.statusCode}`)
        
                                  res.on('data', d => {
                                    console.log(d)
                                    messageString = ""
                                    for(var i=0; i<d.length; i++){
                                      messageString += String.fromCharCode(d[i])
                                    }
                                    messageString = messageString.slice(0,-1)
                                    console.log(messageString)
                                    setMessagesList(messageString.split('\n'))
                                  })
                                })
          
                                req.on('error', error => {
                                  console.error(error)
                                })
        
                                req.write(da)
                                req.end()
                                setChattingFriend(e)
                                setChatting(true)
                                setChatRoom(false)
                              }}>{e}</Button>
                            </p>
                          ))}
                        </div>

                        <Button type="primary" danger onClick={() => {
                          setChatRoom(false)
                        }}>Leave</Button>
                      </>
                      :
                      <>
                        {chatting ? 
                          <>
                            <div className="App-title">
                              <h1>{chattingFriend}</h1>
                            </div>
                            <div className="App-messages">
                              {messagesList.length === 0 ? (
                                <p style={{ color: '#ccc' }}>
                                  {opened? 'No messages...' : 'Loading...'}
                                </p>
                              ) : (
                                messagesList.map((e, i) => (
                                  <p className="App-message" key={i}>
                                    <Tag color="blue">{e}</Tag> {body}
                                  </p>
                                ))
                              )}
                            </div>
                            <Input.Search
                              rows={4}
                              value={body}
                              ref={bodyRef}
                              enterButton="Send"
                              onChange={(e) => setMessages(e.target.value)}
                              placeholder="Type a message here..."
                              onSearch={(msg) => {
                                if (!msg) {
                                  displayStatus({
                                    type: 'error',
                                    msg: 'Please enter a message.'
                                  })
                                  return
                                }
                                else {
                                  const da = `Text${id} ${msg}`
                                  const option = {
                                    hostname: '127.0.0.1',
                                    port: 4000,
                                    method: 'POST',
                                    headers: {
                                      'Content-Type': 'text/plain',
                                      'Content-Length': da.length
                                    }
                                  }
          
                                  const req = http.request(option, res => {
                                    console.log(`statusCode: ${res.statusCode}`)
          
                                    res.on('data', d => {
                                      console.log(d)
                                    })
                                  })
            
                                  req.on('error', error => {
                                    console.error(error)
                                  })
          
                                  req.write(da)
                                  req.end()
                                }
                                setMessages('')
                              }}
                            ></Input.Search>
                            <Button type="primary" danger onClick={() => {
                              setChatting(false)
                              setChatRoom(true)
                            }}>Leave</Button>
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
                                }
                              }

                              const req = http.request(option, res => {
                                console.log(`statusCode: ${res.statusCode}`)

                                res.on('data', d => {
                                  console.log(d)
                                  friendString = ""
                                  for(var i=0; i<d.length; i++){
                                    friendString += String.fromCharCode(d[i])
                                  }
                                  friendString = friendString.slice(0,-1)
                                  console.log(friendString)
                                  setFriends(friendString.split('\n'))
                                })
                              })

                              req.on('error', error => {
                                console.error(error)
                              })

                              req.write(`List friends ${id}`)
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
                              const da = "List friends"
                              const option = {
                                hostname: '127.0.0.1',
                                port: 4000,
                                method: 'POST',
                                headers: {
                                  'Content-Type': 'text/plain',
                                  'Content-Length': da.length,
                                }
                              }

                              const req = http.request(option, res => {
                                console.log(`statusCode: ${res.statusCode}`)

                                res.on('data', d => {
                                  console.log(d)
                                  friendString = ""
                                  for(var i=0; i<d.length; i++){
                                    friendString += String.fromCharCode(d[i])
                                  }
                                  friendString = friendString.slice(0,-1)
                                  console.log(friendString)
                                  setFriends(friendString.split('\n'))
                                })
                              })

                              req.on('error', error => {
                                console.error(error)
                              })

                              req.write(`List friends ${id}`)
                              req.end()
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
        </>
      }
    </div>
  )
}

export default App
