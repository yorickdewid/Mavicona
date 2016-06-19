<?php

namespace Ecoli;

abstract class HttpMethod
{
	const Get = 0;
	const Post = 1;
	const Put = 2;
	const Delete = 3;
}

class Request
{
	/**
	 * Request method.
	 *
	 * @var HttpMethod
	 */
	private $method;
	
	/**
	 * Request uri.
	 *
	 * @var string
	 */
	private $uri;

	/**
	 * Request parameters and query string.
	 *
	 * @var Array
	 */
	private $parameters = [];

	/**
	 * Gather request method.
	 *
	 * @return void
	 */
	private function gatherMethod()
	{
		switch ($_SERVER['REQUEST_METHOD']) {
			case 'PUT':
				$method = HttpMethod::Put;
				break;
			case 'POST':
				$method = HttpMethod::Post;
				break;
			case 'GET':
				$method = HttpMethod::Get;
				break;
			case 'DELETE':
				$method = HttpMethod::Delete;
				break;
			default:
				//handle_error($request);
				break;
		}
	}

	/**
	 * Retrieve URI.
	 *
	 * @return string
	 */
	public function uri()
	{
		if (strlen($this->uri) > 1 && $this->uri[0] == '/')
			return substr($this->uri, 1);

		return $this->uri;
	}

	/**
	 * Retrieve URI.
	 *
	 * @return string
	 */
	public function method()
	{
		switch ($this->method) {
			case HttpMethod::Put:
				return 'put';
			case HttpMethod::Post:
				return 'post';
			case HttpMethod::Get:
				return 'get';
			case HttpMethod::Delete:
				return 'delete';
			default:
				//handle_error($request);
				break;
		}
	}

	/**
	 * Gather request URI.
	 *
	 * @return void
	 */
	private function gatherUri()
	{
		$this->uri = $_SERVER['PATH_INFO'];

		if (empty($this->uri)) {
			$this->uri = '/';
		}
	}

	/**
	 * Gather request query string.
	 *
	 * @return void
	 */
	private function gatherParameters()
	{
		$this->parameters = $_GET;
	}

	/**
	 * Capture all HTTP request properties.
	 *
	 * @return Ecoli\Request
	 */
	public static function capture()
	{
		$request = new Request;

		$request->gatherMethod();
		$request->gatherUri();
		$request->gatherParameters();

		return $request;
	}

}
